#include "parser.hpp"
#include "../utils.hpp"
#include "codes.hpp"
#include "csi_parser.hpp"
#include "osc_parser.hpp"
#include "terminal.hpp"
#include <glm/ext/vector_float2.hpp>
#include <spdlog/fmt/bin_to_hex.h>
#include <spdlog/spdlog.h>

Parser::Parser(CsiParser&& csiParser, OscParser&& oscParser)
    : m_CsiParser(csiParser), m_OscParser(oscParser) {};

std::vector<std::vector<Cell>> Parser::parse(std::vector<uint8_t>& data) {
    std::vector<std::vector<Cell>> rows;
    rows.emplace_back();

    for (auto it = data.begin(); it < data.end(); it++) {
        if (*it == c0::LF || *it == c0::VT || *it == c0::FF) {
            m_State.lineEnd = true;
        }

        switch (*it) {
        // Others to ignore...
        case c0::BS: {
            Terminal::getCursorMut([](glm::vec2& cursor) { cursor.x--; });
            break;
        }

        case c0::CR: {
            Terminal::getCursorMut([](glm::vec2& cursor) { cursor.x = 0; });
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
                return rows;
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
            rows[rows.size() - 1].push_back(Cell{
                .bgColor = m_State.bgColor,
                .fgColor = m_State.fgColor,
                .character = *it,
                .lineStart = m_State.lineStart,
                .lineEnd = m_State.lineEnd,
                .offset = m_State.offset,
            });
            Terminal::getCursorMut([](glm::vec2& cursor) { cursor.x++; });

            if (*it == c0::HT) {
                m_State.offset = 0;
            } else {
                m_State.offset++;
            }

            if (m_State.lineEnd) {
                m_State.offset = 0;
                Terminal::getCursorMut([](glm::vec2& cursor) {
                    cursor.x = 0;
                    cursor.y++;
                });
                rows.emplace_back();
            }
            m_State.lineStart = m_State.lineEnd;
            m_State.lineEnd = false;
            break;
        }
        }
    }

    return rows;
}

// STATIC
std::optional<uint32_t> Parser::parsePs(iter_t& it, iter_t end) {
    std::string digits = "";
    for (; it < end; it++) {
        if (!std::isdigit(*it)) {
            if (digits != "") {
                return std::stoi(digits);
            } else {
                return std::nullopt;
            }
        }
        digits += *it;
    }

    return std::nullopt;
}
