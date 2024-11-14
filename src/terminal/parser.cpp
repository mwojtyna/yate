#include "parser.hpp"
#include "codes.hpp"
#include "csi_parser.hpp"
#include "osc_parser.hpp"
#include "terminal.hpp"
#include "types.hpp"
#include "unicode.hpp"
#include <spdlog/spdlog.h>

Parser::Parser(CsiParser&& csiParser, OscParser&& oscParser)
    : m_CsiParser(csiParser), m_OscParser(oscParser) {};

std::unordered_set<codepoint_t>
Parser::parseAndModifyTermBuf(std::vector<uint8_t>& data, SDL_Window* window) {
    std::unordered_set<codepoint_t> codepoints;

    for (auto it = data.begin(); it < data.end(); it++) {
        codepoint_t codepoint = utf8::decode(it, data.end());
        codepoints.insert(codepoint);

        switch (codepoint) {
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
                m_CsiParser.parse(++it, data.end(), m_State);
                break;
            }
            case c0::OSC: {
                m_OscParser.parse(++it, data.end());
                break;
            }
            case '7': {
                m_State.savedCursorData = Terminal::getCursor();
                break;
            }
            case '8': {
                Terminal::setCursor(m_State.savedCursorData);
                break;
            }
            case 'k': {
                std::string title = readUntilST(++it, data.end());
                SDL_SetWindowTitle(window, title.c_str());
                break;
            }
            case 'M': {
                Terminal::getCursorMut([](cursor_t& cursor) {
                    // TODO: Scroll when needed
                    cursor.y = std::max<float>(cursor.y - 1, 0);
                });
                break;
            }
            default: {
                SPDLOG_WARN("Unsupported escape sequence 'ESC {}' ({:#x})",
                            (char)*it, *it);
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
            glm::vec4 bgColor =
                m_State.inversed ? m_State.fgColor : m_State.bgColor;
            glm::vec4 fgColor =
                m_State.inversed ? m_State.bgColor : m_State.fgColor;
            // Default background has opacity of 0, we have to correct it otherwise when inverted the text would be transparent
            if (m_State.inversed && m_State.bgColor.a == 0) {
                fgColor.a = 1;
            }

            const Cell newCell = Cell{
                .bgColor = bgColor,
                .fgColor = fgColor,
                .character = codepoint,
                .offset = m_State.offset,
            };

            if (!isEol(newCell.character)) {
                Terminal::getCursorMut([&newCell](cursor_t& cursor) {
                    Terminal::getBufMut([&newCell,
                                         &cursor](TerminalBuf& termBuf) {
                        if (!termBuf.getRows().empty()) {
                            std::vector<Cell>& row = termBuf.getRow(cursor.y);
                            if (cursor.x + 1 > row.size()) {
                                row.push_back(std::move(newCell));
                            } else {
                                row[cursor.x] = std::move(newCell);
                            }
                        } else {
                            termBuf.pushRow({std::move(newCell)});
                        }
                    });

                    cursor.x++;
                });
            }

            if (newCell.character == c0::HT) {
                m_State.offset = 0;
            } else {
                m_State.offset++;
            }

            if (isEol(newCell.character)) {
                Terminal::getBufMut([this](TerminalBuf& termBuf) {
                    Terminal::getCursorMut([this, &termBuf](cursor_t& cursor) {
                        // Only add new row when at the last row
                        while (cursor.y + 1 >= termBuf.getRows().size()) {
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
        if (argsString.size() < argIdx + 1) {
            argsString.push_back("");
        }
        argsString[argIdx] += *it;
    }

    return {};
}

std::string Parser::readUntilST(iter_t& it, iter_t end) {
    std::string out = "";

    for (; it < end; it++) {
        out += *it;

        if (*it == c0::ESC && *(it + 1) == '\\') {
            it++;
            return out;
        }
    }

    SPDLOG_ERROR("Buf ended before ST");
    return out;
}

bool Parser::isEol(codepoint_t character) {
    return character == c0::LF || character == c0::VT || character == c0::FF;
}
