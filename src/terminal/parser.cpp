#include "parser.hpp"
#include "../utils.hpp"
#include "constants.hpp"
#include "csi_parser.hpp"
#include "osc_parser.hpp"
#include <cctype>
#include <spdlog/fmt/bin_to_hex.h>
#include <spdlog/spdlog.h>

Parser::Parser(CsiParser& csiParser, OscParser& oscParser)
    : m_CsiParser(csiParser), m_OscParser(oscParser){};

std::vector<Cell> Parser::parse(termbuf_t& data) {
    glm::vec4 bgColor(0);
    glm::vec4 fgColor(1);
    std::vector<Cell> cells;

    for (auto it = data.begin(); it < data.end(); it++) {
        if (*it == '\r') {
            continue;
        }

        switch (*it) {
        // 7-bit
        case ESC: {
            it++;
            switch (*it) {
            case CSI_START: {
                it++;
                m_CsiParser.parse(it, data.end());
                break;
            }
            case OSC_START: {
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
        case OSC: {
            it++;
            m_OscParser.parse(it, data.end());
            break;
        }

        default: {
            cells.push_back(
                Cell{.bgColor = bgColor, .fgColor = fgColor, .character = *it});
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
