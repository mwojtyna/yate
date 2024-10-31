#pragma once

#include "csi_parser.hpp"

namespace csiidents {
/// Erase In Line
inline constexpr CsiIdent EL = CsiIdent{
    .prefix = std::nullopt, .intermediate = std::nullopt, .final = 'K'};

/// Cursor Forward
inline constexpr CsiIdent CUF = CsiIdent{
    .prefix = std::nullopt, .intermediate = std::nullopt, .final = 'C'};

/// Cursor Backward
inline constexpr CsiIdent CUB = CsiIdent{
    .prefix = std::nullopt, .intermediate = std::nullopt, .final = 'D'};

/// Cursor Up
inline constexpr CsiIdent CUU = CsiIdent{
    .prefix = std::nullopt, .intermediate = std::nullopt, .final = 'A'};

/// Cursor Down
inline constexpr CsiIdent CUD = CsiIdent{
    .prefix = std::nullopt, .intermediate = std::nullopt, .final = 'B'};

/// Cursor Horizontal Absolute
inline constexpr CsiIdent CHA = CsiIdent{
    .prefix = std::nullopt, .intermediate = std::nullopt, .final = 'G'};

/// Cursor Position
inline constexpr CsiIdent CUP = CsiIdent{
    .prefix = std::nullopt, .intermediate = std::nullopt, .final = 'H'};

/// Erase In Display
inline constexpr CsiIdent ED = CsiIdent{
    .prefix = std::nullopt, .intermediate = std::nullopt, .final = 'J'};

/// Delete character(s)
inline constexpr CsiIdent DCH = CsiIdent{
    .prefix = std::nullopt, .intermediate = std::nullopt, .final = 'P'};

} // namespace csiidents
