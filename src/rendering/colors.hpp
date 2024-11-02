#pragma once

#include <glm/ext/vector_float4.hpp>

/// Max value is 255
#define RGB(name, r, g, b)                                                     \
    inline constexpr glm::vec4 name =                                          \
        glm::vec4(r / 255.0, g / 255.0, b / 255.0, 1)

namespace colors {
RGB(white, 255, 255, 255);
RGB(black, 0, 0, 0);
RGB(red, 255, 0, 0);
RGB(green, 0, 255, 0);
RGB(blue, 0, 0, 255);
RGB(yellow, 255, 255, 0);
RGB(magenta, 255, 0, 255);
RGB(cyan, 0, 255, 255);
RGB(maroon, 128, 0, 0);
RGB(olive, 128, 128, 0);
RGB(navy, 0, 0, 128);
RGB(purple, 128, 0, 128);

inline constexpr glm::vec4 defaultBg = glm::vec4(0);
inline constexpr glm::vec4 defaultFg = white;
} // namespace colors
