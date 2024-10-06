#pragma once

#include "csi_parser.hpp"
#include "osc_parser.hpp"
#include "terminal_buffer.hpp"
#include <glm/ext/vector_float4.hpp>
#include <vector>

class Parser {
public:
    Parser(CsiParser&& csiParser, OscParser&& oscParser);
    std::vector<std::vector<Cell>> parse(std::vector<uint8_t>& data);

    static std::optional<uint32_t> parsePs(iter_t& it, iter_t end);

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
