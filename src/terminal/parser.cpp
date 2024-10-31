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
        if (*it == c0::LF || *it == c0::VT || *it == c0::FF) {
            m_State.lineEnd = true;
        }

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
                m_CsiParser.parse(it, data.end());
                break;
            }
            case c0::OSC: {
                it++;
                m_OscParser.parse(it, data.end());
                break;
            }
            default: {
                SPDLOG_WARN("Unsupported escape sequence 'ESC {}' in buf:",
                            (char)*it);
                hexdump(data.data(), data.size(), SPDLOG_LEVEL_WARN);
            }
            }
            break;
        }

        // 8-bit
        case c1::CSI: {
            it++;
            m_CsiParser.parse(it, data.end());
            break;
        }
        case c1::OSC: {
            it++;
            m_OscParser.parse(it, data.end());
            break;
        }

        default: {
            codepoints.push_back(*it);

            Terminal::getCursorMut([&](cursor_t& cursor) {
                Terminal::getBufMut([&](TerminalBuf& termBuf) {
                    const Cell newCell = Cell{
                        .bgColor = m_State.bgColor,
                        .fgColor = m_State.fgColor,
                        .character = *it,
                        .lineStart = m_State.lineStart,
                        .lineEnd = m_State.lineEnd,
                        .offset = m_State.offset,
                    };

                    if (!termBuf.getRows().empty()) {
                        std::vector<Cell>& row = termBuf.getRow(cursor.y);

                        if (*it != c0::LF && cursor.x < row.size()) {
                            row.erase(row.begin() + cursor.x, row.end());
                        }

                        // Delete lineEnd Cell if it exists when appending to a row
                        // TODO: More optimized way (hashmap[row] = cell_index_with_lineEnd)
                        for (size_t i = 0; i < row.size(); i++) {
                            if (row[i].lineEnd) {
                                row.erase(row.begin() + i);
                            }
                        }
                        row.push_back(std::move(newCell));
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

            Terminal::getBufMut([this](TerminalBuf& termBuf) {
                Terminal::getCursorMut([this, &termBuf](cursor_t& cursor) {
                    if (m_State.lineEnd) {
                        // Only add new row when at the last row
                        if (cursor.y == termBuf.getRows().size() - 1) {
                            termBuf.pushRow({});
                        }

                        m_State.offset = 0;
                        cursor.x = 0;
                        cursor.y++;
                    }
                });
            });
            m_State.lineStart = m_State.lineEnd;
            m_State.lineEnd = false;
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
