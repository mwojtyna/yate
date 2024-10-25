#pragma once

#include "csi_parser.hpp"
#include <cstdint>
#include <vector>

/*
---------+----------
Home     | CSI 1 ~ |
Insert   | CSI 2 ~ |
Delete   | CSI 3 ~ |
End      | CSI 4 ~ |
PageUp   | CSI 5 ~ |
PageDown | CSI 6 ~ |
---------+----------
*/

namespace csiidents {
static constexpr CsiIdent navKey{
    .prefix = std::nullopt, .intermediate = std::nullopt, .final = '~'};

inline constexpr std::vector<uint8_t> Home() {
    return navKey.data('1');
}
inline constexpr std::vector<uint8_t> Insert() {
    return navKey.data('2');
}
inline constexpr std::vector<uint8_t> Delete() {
    return navKey.data('3');
}
inline constexpr std::vector<uint8_t> End() {
    return navKey.data('4');
}
inline constexpr std::vector<uint8_t> PageUp() {
    return navKey.data('5');
}
inline constexpr std::vector<uint8_t> PageDown() {
    return navKey.data('6');
}
} // namespace csiidents
