#pragma once

#include "csi_parser.hpp"
#include <cstdint>

static constexpr uint8_t ARG_SEPARATOR = ';';

namespace c0 {
/// Null
static constexpr uint8_t NUL = '\0';
/// Bell
static constexpr uint8_t BELL = '\a';
/// Backspace
static constexpr uint8_t BS = '\b';
/// Horizontal Tabulation
static constexpr uint8_t HT = '\t';
/// Line Feed
static constexpr uint8_t LF = '\n';
/// Vertical Tabulation
static constexpr uint8_t VT = '\v';
/// Form Feed
static constexpr uint8_t FF = '\f';
/// Carriage Return
static constexpr uint8_t CR = '\r';
/// Shift Out
static constexpr uint8_t SO = 0x0e;
/// Shift In
static constexpr uint8_t SI = 0x0f;
/// Escape
static constexpr uint8_t ESC = '\e';
/// CSI 7-bit opening character
static constexpr uint8_t CSI = '[';
/// OSC 7-bit opening character
static constexpr uint8_t OSC = ']';
} // namespace c0

namespace c1 {
/// Index
static constexpr uint8_t IND = 0x84;
/// Next Line
static constexpr uint8_t NEL = 0x85;
/// Horizontal Tabulation Set
static constexpr uint8_t HTS = 0x88;
/// DCS 8-bit opening character
static constexpr uint8_t DCS = 0x90;
/// CSI 8-bit opening character
static constexpr uint8_t CSI = 0x9b;
/// String Terminator
static constexpr uint8_t ST = 0x9c;
/// OSC 8-bit opening character
static constexpr uint8_t OSC = 0x9d;
/// Privacy Message
static constexpr uint8_t PM = 0x9e;
/// Application Program Command
static constexpr uint8_t APC = 0x9e;
} // namespace c1

namespace csiidents {
/// Erase In Line
static CsiIdent EL = CsiIdent{
    .prefix = std::nullopt, .intermediate = std::nullopt, .final = 'K'};
} // namespace csiidents
