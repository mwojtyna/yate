#include "parser.hpp"
#include "../utils.hpp"
#include "codes.hpp"
#include "csi_parser.hpp"
#include "osc_parser.hpp"
#include <cctype>
#include <spdlog/fmt/bin_to_hex.h>
#include <spdlog/spdlog.h>

Parser::Parser(CsiParser& csiParser, OscParser& oscParser)
    : m_CsiParser(csiParser), m_OscParser(oscParser){};

std::vector<Cell> Parser::parse(std::vector<uint8_t>& data) {
    std::vector<Cell> cells;

    for (auto it = data.begin(); it < data.end(); it++) {
        if (*it == c0::LF || *it == c0::VT || *it == c0::FF) {
            m_State.lineEnd = true;
        }
        if (*it == c0::CR) {
            // TODO: Return cursor to line beginning
            continue;
        }

        switch (*it) {
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
                return cells;
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
            cells.push_back(Cell{
                .bgColor = m_State.bgColor,
                .fgColor = m_State.fgColor,
                .character = *it,
                .lineStart = m_State.lineStart,
                .lineEnd = m_State.lineEnd,
                .offset = m_State.offset,
            });
            if (*it == c0::HT) {
                m_State.offset = 0;
            } else {
                m_State.offset++;
            }

            if (m_State.lineEnd) {
                m_State.offset = 0;
            }
            m_State.lineStart = m_State.lineEnd;
            m_State.lineEnd = false;
            break;
        }
        }
    }

    return cells;
}

// STATIC
std::optional<uint32_t> Parser::parsePs(iter_t& it, iter_t end) {
    std::string digits = "";

    skipSpaces(it, end);
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

void Parser::skipSpaces(iter_t& it, iter_t end) {
    size_t spacesSkipped = 0;
    for (; it < end; it++) {
        if (*it != ' ') {
            return;
        }
    }
}
