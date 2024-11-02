#include "parser.hpp"
#include "../utils.hpp"
#include "codes.hpp"
#include "csi_parser.hpp"
#include "osc_parser.hpp"
#include "terminal.hpp"
#include "types.hpp"
#include <spdlog/spdlog.h>

Parser::Parser(CsiParser&& csiParser, OscParser&& oscParser)
    : m_CsiParser(csiParser), m_OscParser(oscParser) {};

std::vector<codepoint_t>
Parser::parseAndModifyTermBuf(std::vector<uint8_t>& data) {
    std::vector<codepoint_t> codepoints(data.size());

    for (auto it = data.begin(); it < data.end(); it++) {
        switch (*it) {
        case c0::NUL: {
            continue;
        }

        case c0::BS: {
            Terminal::getCursorMut([](cursor_t& cursor) { cursor.x--; });
            break;
        }

        case c0::CR: {
            Terminal::getCursorMut([](cursor_t& cursor) { cursor.x = 0; });
            break;
        }
        case c0::BEL: {
            // TODO: Sound bell
            break;
        }

        // 7-bit
        case c0::ESC: {
            it++;
            switch (*it) {
            case c0::CSI: {
                it++;
                m_CsiParser.parse(it, data.end(), m_State);
                break;
            }
            case c0::OSC: {
                it++;
                m_OscParser.parse(it, data.end());
                break;
            }
            default: {
                SPDLOG_WARN(
                    "Unsupported escape sequence 'ESC {}' (0x{:x}) in buf:",
                    (char)*it, *it);
                hexdump(data.data(), data.size(), SPDLOG_LEVEL_WARN);
            }
            }
            break;
        }

        // 8-bit
        case c1::CSI: {
            it++;
            m_CsiParser.parse(it, data.end(), m_State);
            break;
        }
        case c1::OSC: {
            it++;
            m_OscParser.parse(it, data.end());
            break;
        }

        default: {
            codepoints.push_back(*it);

            const Cell newCell = Cell{
                .bgColor = m_State.bgColor,
                .fgColor = m_State.fgColor,
                .character = *it,
                .offset = m_State.offset,
            };
            Terminal::getCursorMut([this, &newCell, &it](cursor_t& cursor) {
                Terminal::getBufMut([this, &newCell, &cursor,
                                     &it](TerminalBuf& termBuf) {
                    if (!termBuf.getRows().empty()) {
                        std::vector<Cell>& row = termBuf.getRow(cursor.y);

                        // Delete all characters in the row after this character when appending to this row
                        if (!isEol(*it) && cursor.x < row.size()) {
                            row.erase(row.begin() + cursor.x, row.end());
                        }

                        // Change '\n' cell to empty cell if it exists when appending to this row
                        std::optional<size_t> eolIndex =
                            termBuf.getEolIndexInRow(cursor.y);
                        if (eolIndex.has_value()) {
                            if (eolIndex.value() < row.size()) {
                                row[eolIndex.value()] = Cell::empty();
                            }
                            termBuf.deleteEolIndexInRow(cursor.y);
                        }

                        row.push_back(std::move(newCell));
                        if (isEol(*it)) {
                            // Needed to avoid cursor jumping to next line in some cases
                            // e.g. node REPL `console.log()`
                            row.insert(row.end() - 1, Cell::empty());
                            termBuf.setEolIndexInRow(cursor.y, row.size() - 1);
                        }
                    } else {
                        termBuf.pushRow({std::move(newCell)});
                    }
                });

                cursor.x++;
            });

            if (*it == c0::HT) {
                m_State.offset = 0;
            } else {
                m_State.offset++;
            }

            if (isEol(*it)) {
                Terminal::getBufMut([this, &it](TerminalBuf& termBuf) {
                    Terminal::getCursorMut(
                        [this, &termBuf, &it](cursor_t& cursor) {
                            // Only add new row when at the last row
                            if (cursor.y == termBuf.getRows().size() - 1) {
                                termBuf.pushRow({});
                            }

                            m_State.offset = 0;
                            cursor.x = 0;
                            cursor.y++;
                        });
                });
            }
            break;
        }
        }
    }

    return codepoints;
}

// STATIC
std::vector<uint32_t> Parser::parsePs(iter_t& it, iter_t end) {
    std::vector<std::string> argsString;

    for (uint8_t argIdx = 0; it < end; it++) {
        if (*it == ARG_SEPARATOR) {
            argIdx++;
            continue;
        }
        // If we have gone past the Ps array
        if (!std::isdigit(*it)) {
            std::vector<uint32_t> args(argsString.size());
            for (uint8_t i = 0; i < argsString.size(); i++) {
                args[i] = std::stoi(argsString[i]);
            }
            return args;
        }

        // Add new argument to list if argIdx has been incremented
        if (argsString.empty() || argsString.size() - 1 < argIdx) {
            argsString.push_back("");
        }
        argsString[argIdx] += *it;
    }

    return {};
}

bool Parser::isEol(codepoint_t character) {
    return character == c0::LF || character == c0::VT || character == c0::FF;
}
