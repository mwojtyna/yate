#pragma once

#include "../rendering/font.hpp"
#include "glm/ext/vector_float4.hpp"
#include "terminal.hpp"
#include <vector>

struct ParsedChunk {
    glm::vec4 bgColor;
    glm::vec4 fgColor;
    std::vector<Codepoint> text;
};

class TerminalParser {
public:
    std::vector<ParsedChunk> parse(TerminalRaw& rawCodes);

private:
};
