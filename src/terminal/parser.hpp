#pragma once

#include "../rendering/font.hpp"
#include "glm/ext/vector_float4.hpp"
#include "osc_parser.hpp"
#include <optional>
#include <vector>

struct CellChunk {
    glm::vec4 bgColor;
    glm::vec4 fgColor;
    std::vector<Codepoint> text;
};

class Parser {
public:
    Parser(OscParser& oscParser);
    std::vector<CellChunk> parse(std::vector<uint8_t>& data);

    static std::optional<uint32_t> parsePs(std::vector<uint8_t>::iterator& it,
                                           std::vector<uint8_t>::iterator end);

    static void skipSpaces(std::vector<uint8_t>::iterator& it,
                           std::vector<uint8_t>::iterator end);

private:
    OscParser m_OscParser;
};
