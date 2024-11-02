#pragma once

#include <glm/ext/vector_float4.hpp>

#define RGB(r, g, b) glm::vec4(r / 255.0, g / 255.0, b / 255.0, 1)

namespace colors {
inline constexpr glm::vec4 white = RGB(255, 255, 255);
inline constexpr glm::vec4 black = RGB(0, 0, 0);
inline constexpr glm::vec4 red = RGB(255, 0, 0);
inline constexpr glm::vec4 green = RGB(0, 255, 0);
inline constexpr glm::vec4 blue = RGB(0, 0, 255);
inline constexpr glm::vec4 yellow = RGB(255, 255, 0);
inline constexpr glm::vec4 magenta = RGB(255, 0, 255);
inline constexpr glm::vec4 cyan = RGB(0, 255, 255);

inline constexpr glm::vec4 defaultBg = glm::vec4(0); // Must be transparent
inline constexpr glm::vec4 defaultFg = white;

// Generated with scripts/fetch_colors/main.py
inline constexpr glm::vec4 colors256[256] = {
    RGB(0, 0, 0),       RGB(128, 0, 0),     RGB(0, 128, 0),
    RGB(128, 128, 0),   RGB(0, 0, 128),     RGB(128, 0, 128),
    RGB(0, 128, 128),   RGB(192, 192, 192), RGB(128, 128, 128),
    RGB(255, 0, 0),     RGB(0, 255, 0),     RGB(255, 255, 0),
    RGB(0, 0, 255),     RGB(255, 0, 255),   RGB(0, 255, 255),
    RGB(255, 255, 255), RGB(0, 0, 0),       RGB(0, 0, 95),
    RGB(0, 0, 135),     RGB(0, 0, 175),     RGB(0, 0, 215),
    RGB(0, 0, 255),     RGB(0, 95, 0),      RGB(0, 95, 95),
    RGB(0, 95, 135),    RGB(0, 95, 175),    RGB(0, 95, 215),
    RGB(0, 95, 255),    RGB(0, 135, 0),     RGB(0, 135, 95),
    RGB(0, 135, 135),   RGB(0, 135, 175),   RGB(0, 135, 215),
    RGB(0, 135, 255),   RGB(0, 175, 0),     RGB(0, 175, 95),
    RGB(0, 175, 135),   RGB(0, 175, 175),   RGB(0, 175, 215),
    RGB(0, 175, 255),   RGB(0, 215, 0),     RGB(0, 215, 95),
    RGB(0, 215, 135),   RGB(0, 215, 175),   RGB(0, 215, 215),
    RGB(0, 215, 255),   RGB(0, 255, 0),     RGB(0, 255, 95),
    RGB(0, 255, 135),   RGB(0, 255, 175),   RGB(0, 255, 215),
    RGB(0, 255, 255),   RGB(95, 0, 0),      RGB(95, 0, 95),
    RGB(95, 0, 135),    RGB(95, 0, 175),    RGB(95, 0, 215),
    RGB(95, 0, 255),    RGB(95, 95, 0),     RGB(95, 95, 95),
    RGB(95, 95, 135),   RGB(95, 95, 175),   RGB(95, 95, 215),
    RGB(95, 95, 255),   RGB(95, 135, 0),    RGB(95, 135, 95),
    RGB(95, 135, 135),  RGB(95, 135, 175),  RGB(95, 135, 215),
    RGB(95, 135, 255),  RGB(95, 175, 0),    RGB(95, 175, 95),
    RGB(95, 175, 135),  RGB(95, 175, 175),  RGB(95, 175, 215),
    RGB(95, 175, 255),  RGB(95, 215, 0),    RGB(95, 215, 95),
    RGB(95, 215, 135),  RGB(95, 215, 175),  RGB(95, 215, 215),
    RGB(95, 215, 255),  RGB(95, 255, 0),    RGB(95, 255, 95),
    RGB(95, 255, 135),  RGB(95, 255, 175),  RGB(95, 255, 215),
    RGB(95, 255, 255),  RGB(135, 0, 0),     RGB(135, 0, 95),
    RGB(135, 0, 135),   RGB(135, 0, 175),   RGB(135, 0, 215),
    RGB(135, 0, 255),   RGB(135, 95, 0),    RGB(135, 95, 95),
    RGB(135, 95, 135),  RGB(135, 95, 175),  RGB(135, 95, 215),
    RGB(135, 95, 255),  RGB(135, 135, 0),   RGB(135, 135, 95),
    RGB(135, 135, 135), RGB(135, 135, 175), RGB(135, 135, 215),
    RGB(135, 135, 255), RGB(135, 175, 0),   RGB(135, 175, 95),
    RGB(135, 175, 135), RGB(135, 175, 175), RGB(135, 175, 215),
    RGB(135, 175, 255), RGB(135, 215, 0),   RGB(135, 215, 95),
    RGB(135, 215, 135), RGB(135, 215, 175), RGB(135, 215, 215),
    RGB(135, 215, 255), RGB(135, 255, 0),   RGB(135, 255, 95),
    RGB(135, 255, 135), RGB(135, 255, 175), RGB(135, 255, 215),
    RGB(135, 255, 255), RGB(175, 0, 0),     RGB(175, 0, 95),
    RGB(175, 0, 135),   RGB(175, 0, 175),   RGB(175, 0, 215),
    RGB(175, 0, 255),   RGB(175, 95, 0),    RGB(175, 95, 95),
    RGB(175, 95, 135),  RGB(175, 95, 175),  RGB(175, 95, 215),
    RGB(175, 95, 255),  RGB(175, 135, 0),   RGB(175, 135, 95),
    RGB(175, 135, 135), RGB(175, 135, 175), RGB(175, 135, 215),
    RGB(175, 135, 255), RGB(175, 175, 0),   RGB(175, 175, 95),
    RGB(175, 175, 135), RGB(175, 175, 175), RGB(175, 175, 215),
    RGB(175, 175, 255), RGB(175, 215, 0),   RGB(175, 215, 95),
    RGB(175, 215, 135), RGB(175, 215, 175), RGB(175, 215, 215),
    RGB(175, 215, 255), RGB(175, 255, 0),   RGB(175, 255, 95),
    RGB(175, 255, 135), RGB(175, 255, 175), RGB(175, 255, 215),
    RGB(175, 255, 255), RGB(215, 0, 0),     RGB(215, 0, 95),
    RGB(215, 0, 135),   RGB(215, 0, 175),   RGB(215, 0, 215),
    RGB(215, 0, 255),   RGB(215, 95, 0),    RGB(215, 95, 95),
    RGB(215, 95, 135),  RGB(215, 95, 175),  RGB(215, 95, 215),
    RGB(215, 95, 255),  RGB(215, 135, 0),   RGB(215, 135, 95),
    RGB(215, 135, 135), RGB(215, 135, 175), RGB(215, 135, 215),
    RGB(215, 135, 255), RGB(215, 175, 0),   RGB(215, 175, 95),
    RGB(215, 175, 135), RGB(215, 175, 175), RGB(215, 175, 215),
    RGB(215, 175, 255), RGB(215, 215, 0),   RGB(215, 215, 95),
    RGB(215, 215, 135), RGB(215, 215, 175), RGB(215, 215, 215),
    RGB(215, 215, 255), RGB(215, 255, 0),   RGB(215, 255, 95),
    RGB(215, 255, 135), RGB(215, 255, 175), RGB(215, 255, 215),
    RGB(215, 255, 255), RGB(255, 0, 0),     RGB(255, 0, 95),
    RGB(255, 0, 135),   RGB(255, 0, 175),   RGB(255, 0, 215),
    RGB(255, 0, 255),   RGB(255, 95, 0),    RGB(255, 95, 95),
    RGB(255, 95, 135),  RGB(255, 95, 175),  RGB(255, 95, 215),
    RGB(255, 95, 255),  RGB(255, 135, 0),   RGB(255, 135, 95),
    RGB(255, 135, 135), RGB(255, 135, 175), RGB(255, 135, 215),
    RGB(255, 135, 255), RGB(255, 175, 0),   RGB(255, 175, 95),
    RGB(255, 175, 135), RGB(255, 175, 175), RGB(255, 175, 215),
    RGB(255, 175, 255), RGB(255, 215, 0),   RGB(255, 215, 95),
    RGB(255, 215, 135), RGB(255, 215, 175), RGB(255, 215, 215),
    RGB(255, 215, 255), RGB(255, 255, 0),   RGB(255, 255, 95),
    RGB(255, 255, 135), RGB(255, 255, 175), RGB(255, 255, 215),
    RGB(255, 255, 255), RGB(8, 8, 8),       RGB(18, 18, 18),
    RGB(28, 28, 28),    RGB(38, 38, 38),    RGB(48, 48, 48),
    RGB(58, 58, 58),    RGB(68, 68, 68),    RGB(78, 78, 78),
    RGB(88, 88, 88),    RGB(98, 98, 98),    RGB(108, 108, 108),
    RGB(118, 118, 118), RGB(128, 128, 128), RGB(138, 138, 138),
    RGB(148, 148, 148), RGB(158, 158, 158), RGB(168, 168, 168),
    RGB(178, 178, 178), RGB(188, 188, 188), RGB(198, 198, 198),
    RGB(208, 208, 208), RGB(218, 218, 218), RGB(228, 228, 228),
    RGB(238, 238, 238),
};
} // namespace colors