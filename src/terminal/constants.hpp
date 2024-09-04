#include <cstdint>

// Characters
static constexpr uint8_t ESC = 0x1b;
static constexpr uint8_t CSI_START = '[';
static constexpr uint8_t OSC_START = ']';
static constexpr uint8_t SEPARATOR = ';';
static constexpr uint8_t BELL = 0x7;

// C1
static constexpr uint8_t ST = 0x9c;
