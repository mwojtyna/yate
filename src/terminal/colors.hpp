#pragma once

#include <glm/ext/vector_float4.hpp>

namespace colors {
inline constexpr glm::vec4 transparent = glm::vec4(0);
inline constexpr glm::vec4 white = glm::vec4(1);
inline constexpr glm::vec4 black = glm::vec4(0, 0, 0, 1);
inline constexpr glm::vec4 red = glm::vec4(1, 0, 0, 1);
inline constexpr glm::vec4 green = glm::vec4(0, 1, 0, 1);
inline constexpr glm::vec4 blue = glm::vec4(0, 0, 1, 1);
inline constexpr glm::vec4 yellow = glm::vec4(1, 1, 0, 1);
inline constexpr glm::vec4 magenta = glm::vec4(1, 0, 1, 1);
inline constexpr glm::vec4 cyan = glm::vec4(0, 1, 1, 1);

inline constexpr glm::vec4 defaultBg = transparent;
inline constexpr glm::vec4 defaultFg = white;
} // namespace colors
