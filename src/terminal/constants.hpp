#include <cstdint>

static constexpr uint8_t ARG_SEPARATOR = ';';

namespace c0 {
static constexpr uint8_t BELL = 0x7;
static constexpr uint8_t ESC = 0x1b;

static constexpr uint8_t CSI = '[';
static constexpr uint8_t OSC = ']';
} // namespace c0

namespace c1 {
static constexpr uint8_t ST = 0x9c;
static constexpr uint8_t OSC = 0x9d;
} // namespace c1
