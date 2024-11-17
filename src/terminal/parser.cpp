#include "parser.hpp"
#include "codes.hpp"
#include "csi_parser.hpp"
#include "osc_parser.hpp"
#include "types.hpp"
#include "unicode.hpp"
#include <spdlog/spdlog.h>

Parser::Parser(CsiParser&& csiParser, OscParser&& oscParser,
               EscParser&& escParser)
    : m_CsiParser(csiParser), m_OscParser(oscParser), m_EscParser(escParser) {};

std::unordered_set<codepoint_t> Parser::parse(std::vector<uint8_t>& data,
                                              TerminalBuf& termBuf,
                                              cursor_t& cursor) {
    std::unordered_set<codepoint_t> codepoints;

    for (auto it = data.begin(); it < data.end(); it++) {
        codepoint_t codepoint = utf8::decode(it, data.end());
        codepoints.insert(codepoint);

        switch (codepoint) {
        case c0::NUL: {
            continue;
        }

        case c0::BS: {
            cursor.x--;
            break;
        }

        case c0::CR: {
            cursor.x = 0;
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
                m_CsiParser.parse(it, data.end(), m_State, termBuf, cursor);
                break;
            }
            case c0::OSC: {
                it++;
                m_OscParser.parse(it, data.end(), termBuf, cursor);
                break;
            }
            default: {
                m_EscParser.parse(it, data.end(), m_State, termBuf, cursor);
            }
            }
            break;
        }

        // 8-bit
        case c1::CSI: {
            it++;
            m_CsiParser.parse(it, data.end(), m_State, termBuf, cursor);
            break;
        }
        case c1::OSC: {
            it++;
            m_OscParser.parse(it, data.end(), termBuf, cursor);
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

                cursor.x++;
            }

            if (newCell.character == c0::HT) {
                m_State.offset = 0;
            } else {
                m_State.offset++;
            }

            if (isEol(newCell.character)) {
                // Only add new row when at the last row
                while (cursor.y + 1 >= termBuf.getRows().size()) {
                    termBuf.pushRow({});
                }

                m_State.offset = 0;
                cursor.x = 0;
                cursor.y++;
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

        if (*it == c0::ESC && *(it + 1) == c0::ST) {
            it++;
            return out;
        } else if (*it == c1::ST) {
            return out;
        }
    }

    SPDLOG_ERROR("Buf ended before ST");
    return out;
}

bool Parser::isEol(codepoint_t character) {
    return character == c0::LF || character == c0::VT || character == c0::FF;
}
