#pragma once

#include "../rendering/font.hpp"
#include "glm/ext/vector_float4.hpp"
#include <cstdint>
#include <vector>

using iter_t = std::vector<uint8_t>::iterator;

struct Cell {
    glm::vec4 bgColor;
    glm::vec4 fgColor;
    codepoint_t character;
};
