#pragma once

#include "../rendering/colors.hpp"
#include <cstdint>
#include <glm/ext/vector_float2.hpp>
#include <glm/ext/vector_float4.hpp>
#include <spdlog/spdlog.h>
#include <vector>

using iter_t = std::vector<uint8_t>::iterator;
using cursor_t = glm::vec2;

struct ParserState {
    glm::vec4 bgColor = colors::defaultBg;
    glm::vec4 fgColor = colors::defaultFg;
    bool inversed = false;
    size_t offset = 0;
};
