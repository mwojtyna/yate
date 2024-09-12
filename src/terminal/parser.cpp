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

std::vector<CellChunk> Parser::parse(termbuf_t& data) {
    std::vector<CellChunk> chunks;
    size_t chunkIdx = 0;
    chunks.push_back(
        CellChunk{.bgColor = glm::vec4(0), .fgColor = glm::vec4(1)});

    // TODO: Split into chunks when color is different
    for (auto it = data.begin(); it < data.end(); it++) {
        switch (*it) {
        // 7-bit
        case ESC: {
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
            break;
        }

        // 8-bit
        case OSC: {
            it++;
            m_OscParser.parse(it, data.end());
            break;
        }

        default: {
            chunks[chunkIdx].text.push_back(*it);
            break;
        }
        }
    }

    return chunks;
}

// STATIC
std::optional<uint32_t> Parser::parsePs(iter_t& it, iter_t end) {
    std::string digits = "";

    skipSpaces(it, end);
    for (; it < end; it++) {
        if (!std::isdigit(*it)) {
            if (*it == SEPARATOR) {
                if (digits != "") {
                    return std::stoi(digits);
                } else {
                    return std::nullopt;
                }
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
