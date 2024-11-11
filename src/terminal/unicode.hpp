#pragma once

#include "types.hpp"
#include <cstdint>
#include <vector>

using codepoint_t = uint32_t;

namespace utf8 {
std::vector<uint8_t> encode(codepoint_t codepoint);
codepoint_t decode(std::vector<uint8_t>& encoded);
codepoint_t decode(iter_t& it, iter_t end);
} // namespace utf8
