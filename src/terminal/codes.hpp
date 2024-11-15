#pragma once

#include <cstdint>

inline constexpr uint8_t ARG_SEPARATOR = ';';

namespace c0 {
/// Null
inline constexpr uint8_t NUL = '\0';
/// Bell
inline constexpr uint8_t BEL = '\a';
/// Backspace
inline constexpr uint8_t BS = '\b';
/// Horizontal Tabulation
inline constexpr uint8_t HT = '\t';
/// Line Feed
inline constexpr uint8_t LF = '\n';
/// Vertical Tabulation
inline constexpr uint8_t VT = '\v';
/// Form Feed
inline constexpr uint8_t FF = '\f';
/// Carriage Return
inline constexpr uint8_t CR = '\r';
/// Shift Out
inline constexpr uint8_t SO = 0x0e;
/// Shift In
inline constexpr uint8_t SI = 0x0f;
/// Escape
inline constexpr uint8_t ESC = '\e';
/// CSI 7-bit opening character
inline constexpr uint8_t CSI = '[';
/// OSC 7-bit opening character
inline constexpr uint8_t OSC = ']';
// String terminator, 7-bit version (MUST BE PRECEDED BY ESC)
inline constexpr uint8_t ST = '\\';
} // namespace c0

namespace c1 {
/// Index
inline constexpr uint8_t IND = 0x84;
/// Next Line
inline constexpr uint8_t NEL = 0x85;
/// Horizontal Tabulation Set
inline constexpr uint8_t HTS = 0x88;
/// DCS 8-bit opening character
inline constexpr uint8_t DCS = 0x90;
/// CSI 8-bit opening character
inline constexpr uint8_t CSI = 0x9b;
/// String Terminator
inline constexpr uint8_t ST = 0x9c;
/// OSC 8-bit opening character
inline constexpr uint8_t OSC = 0x9d;
/// Privacy Message
inline constexpr uint8_t PM = 0x9e;
/// Application Program Command
inline constexpr uint8_t APC = 0x9f;
} // namespace c1
