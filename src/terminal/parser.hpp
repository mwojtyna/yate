#pragma once

#include "csi_parser.hpp"
#include "osc_parser.hpp"
#include <glm/ext/vector_float4.hpp>
#include <unordered_set>

class Parser {
public:
    Parser(CsiParser&& csiParser, OscParser&& oscParser);

    /// Returns: list of parsed character codepoints
    std::unordered_set<codepoint_t>
    parseAndModifyTermBuf(std::vector<uint8_t>& data);

    static std::vector<uint32_t> parsePs(iter_t& it, iter_t end);

private:
    struct ParserState {
        glm::vec4 bgColor = glm::vec4(0);
        glm::vec4 fgColor = glm::vec4(1);
        bool lineStart = true;
        bool lineEnd = false;
        size_t offset = 0;
    };

    ParserState m_State;
    CsiParser m_CsiParser;
    OscParser m_OscParser;
};
