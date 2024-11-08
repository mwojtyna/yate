#pragma once

#include "csi_parser.hpp"

namespace csiidents {
/// Insert Characters
inline constexpr CsiIdent ICH = CsiIdent{
    .prefix = std::nullopt, .intermediate = std::nullopt, .final = '@'};

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

/// Horizontal Position Absolute
inline constexpr CsiIdent HPA = CsiIdent{
    .prefix = std::nullopt, .intermediate = std::nullopt, .final = '`'};

/// Horizontal Position Relative
inline constexpr CsiIdent HPR = CsiIdent{
    .prefix = std::nullopt, .intermediate = std::nullopt, .final = 'a'};

/// Horizontal and Vertical Position
inline constexpr CsiIdent HVP = CsiIdent{
    .prefix = std::nullopt, .intermediate = std::nullopt, .final = 'f'};

/// Select Graphic Rendition
inline constexpr CsiIdent SGR = CsiIdent{
    .prefix = std::nullopt, .intermediate = std::nullopt, .final = 'm'};

} // namespace csiidents
