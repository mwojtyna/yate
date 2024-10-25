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

inline constexpr CsiIdent DCH = CsiIdent{
    .prefix = std::nullopt, .intermediate = std::nullopt, .final = 'P'};

} // namespace csiidents
