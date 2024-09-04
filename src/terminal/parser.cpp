#include "parser.hpp"
#include "../utils.hpp"
#include "constants.hpp"
#include "osc_parser.hpp"
#include <cctype>
#include <optional>
#include <spdlog/fmt/bin_to_hex.h>
#include <spdlog/spdlog.h>
#include <string>

Parser::Parser(OscParser& oscParser) : m_OscParser(oscParser){};

std::vector<CellChunk> Parser::parse(std::vector<uint8_t>& data) {
    std::vector<CellChunk> chunks;

    for (auto it = data.begin(); it <= data.end(); it++) {
        // TODO: Allow c1 escape codes (0x9d for OSC, etc.)
        if (*it == ESC) {
            it++;
            switch (*it) {
            case OSC_START: {
                it++;
                m_OscParser.parse(it, data.end());
                break;
            }
            default: {
                SPDLOG_WARN("Unsupported escape sequence 'ESC {}' in buf:",
                            (char)*it);
                hexdump(data.data(), data.size(), SPDLOG_LEVEL_WARN);
                return chunks;
            }
            }
        }
    }

    return chunks;
}

// STATIC
std::optional<uint32_t> Parser::parsePs(std::vector<uint8_t>::iterator& it,
                                        std::vector<uint8_t>::iterator end) {
    std::string digits;

    skipSpaces(it, end);
    for (; it < end; it++) {
        if (!std::isdigit(*it)) {
            if (*it == SEPARATOR) {
                return std::stoi(digits);
            } else {
                return std::nullopt;
            }
        }
        digits += *it;
    }

    return std::nullopt;
}

void Parser::skipSpaces(std::vector<uint8_t>::iterator& it,
                        std::vector<uint8_t>::iterator end) {
    size_t spacesSkipped = 0;
    for (; it < end; it++) {
        if (*it != ' ') {
            return;
        }
    }
}
