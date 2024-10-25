#pragma once

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
inline constexpr std::vector<uint8_t> Home();
inline constexpr std::vector<uint8_t> Insert();
inline constexpr std::vector<uint8_t> Delete();
inline constexpr std::vector<uint8_t> End();
inline constexpr std::vector<uint8_t> PageUp();
inline constexpr std::vector<uint8_t> PageDown();
} // namespace csiidents
