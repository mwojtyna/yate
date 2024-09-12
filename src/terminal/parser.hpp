#pragma once

#include "../rendering/font.hpp"
#include "osc_parser.hpp"
#include <glm/ext/vector_float4.hpp>
#include <optional>
#include <vector>

struct CellChunk {
    glm::vec4 bgColor;
    glm::vec4 fgColor;
    std::vector<codepoint_t> text;
};

class Parser {
public:
    Parser(OscParser& oscParser);
    std::vector<CellChunk> parse(termbuf_t& data);

    static std::optional<uint32_t> parsePs(iter_t& it, iter_t end);
    static void skipSpaces(iter_t& it, iter_t end);

private:
    OscParser m_OscParser;
};
