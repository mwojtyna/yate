#include "unicode.hpp"
#include "../rendering/font.hpp"
#include "types.hpp"
#include <spdlog/spdlog.h>

namespace utf8 {
std::vector<uint8_t> encode(codepoint_t c) {
    if (c >= 0xd800 & c <= 0xdfff) {
        SPDLOG_ERROR("Prohibited codepoint value of {:#x}", c);
        return encode(Font::REPLACEMENT_CHAR);
    }

    uint8_t bytesRequired = 0;
    if (c >= 0x0 && c <= 0x7f) {
        bytesRequired = 1;
    } else if (c >= 0x80 && c <= 0x7ff) {
        bytesRequired = 2;
    } else if (c >= 0x800 && c <= 0xffff) {
        bytesRequired = 3;
    } else if (c >= 0x10000 && c <= 0x10ffff) {
        bytesRequired = 4;
    } else {
        SPDLOG_ERROR("Invalid codepoint value of {:#x}", c);
        return encode(Font::REPLACEMENT_CHAR);
    }

    std::vector<uint8_t> encoded(bytesRequired);
    switch (bytesRequired) {
    case 1: {
        encoded[0] = c;
        break;
    }
    case 2: {
        encoded[1] = 0b10'000000 | (c & 0b00'111111);
        encoded[0] = 0b110'00000 | (c >> 6 & 0b000'11111);
        break;
    }
    case 3: {
        encoded[2] = 0b10'000000 | (c & 0b00'111111);
        encoded[1] = 0b10'000000 | (c >> 6 & 0b00'111111);
        encoded[0] = 0b1110'0000 | (c >> 12 & 0b0000'1111);
        break;
    }
    case 4: {
        encoded[3] = 0b10'000000 | (c & 0b00'111111);
        encoded[2] = 0b10'000000 | (c >> 6 & 0b00'111111);
        encoded[1] = 0b10'000000 | (c >> 12 & 0b00'111111);
        encoded[0] = 0b11110'000 | (c >> 18 & 0b00000'111);
        break;
    }
    }

    return encoded;
}

codepoint_t decode(std::vector<uint8_t>& encoded) {
    if (encoded.empty()) {
        SPDLOG_ERROR("UTF-8 sequence empty");
        return Font::REPLACEMENT_CHAR;
    }
    if (encoded.size() > 4) {
        SPDLOG_ERROR("UTF-8 sequence with length={} is too long",
                     encoded.size());
        return Font::REPLACEMENT_CHAR;
    }

    if (encoded.size() == 1) {
        if ((encoded[0] & 0b1'0000000) >> 7 == 0b0) {
            return encoded[0];
        } else {
            SPDLOG_ERROR("Invalid UTF-8 header {:#b} in sequence of size {}",
                         encoded[0], encoded.size());
            return Font::REPLACEMENT_CHAR;
        }
    }

    codepoint_t codepoint = Font::REPLACEMENT_CHAR;
    for (int8_t i = encoded.size() - 1; i >= 0; i--) {
        uint8_t octet = encoded[i];

        if (octet == 0xc0 || octet == 0xc1 || octet == 0xf5 || octet == 0xff) {
            SPDLOG_ERROR("Prohibited UTF-8 octet={:#x} with index={}", octet,
                         i);
            return Font::REPLACEMENT_CHAR;
        }

        if (i > 0) {
            if ((octet & 0b11'000000) >> 6 != 0b10) {
                SPDLOG_ERROR("Invalid UTF-8 octet={:#b} with index={}", octet,
                             i);
                return Font::REPLACEMENT_CHAR;
            }
            codepoint |= (octet & 0b00'111111)
                         << (6 * (encoded.size() - (i + 1)));
        } else {
            // Decode header
            uint8_t mask = 0b11111111 >> (encoded.size() + 1);
            uint8_t header = (octet & ~mask) >> (8 - (encoded.size() + 1));
            uint8_t expectedHeader =
                (0b11111111 >> (8 - (encoded.size() + 1))) & 0b11111110;

            if (header != expectedHeader) {
                SPDLOG_ERROR(
                    "Invalid UTF-8 header {:#b} in sequence of size {}", octet,
                    encoded.size());
                return Font::REPLACEMENT_CHAR;
            }

            codepoint |= (octet & mask) << (6 * (encoded.size() - 1));
        }
    }

    return codepoint;
}

codepoint_t decode(iter_t& it, iter_t end) {
    codepoint_t codepoint = 0;

    uint8_t bytesRequired = 0;
    if ((*it & 0b1'0000000) >> 7 == 0b0) {
        codepoint = *it;
        return codepoint;
    } else if ((*it & 0b111'00000) >> 5 == 0b110) {
        bytesRequired = 2;
        codepoint |= (*it & 0b000'11111) << 6;
    } else if ((*it & 0b1111'0000) >> 4 == 0b1110) {
        bytesRequired = 3;
        codepoint |= (*it & 0b0000'1111) << 6 * 2;
    } else if ((*it & 0b11111'000) >> 3 == 0b11110) {
        bytesRequired = 4;
        codepoint |= (*it & 0b00000'111) << 6 * 3;
    } else {
        SPDLOG_ERROR("Invalid UTF-8 header {:#b}", *it);
        return Font::REPLACEMENT_CHAR;
    }
    it++;

    int8_t i = 1;
    for (; i < bytesRequired && it < end; i++, it++) {
        if (*it == 0xc0 || *it == 0xc1 || *it == 0xf5 || *it == 0xff) {
            SPDLOG_ERROR("Prohibited UTF-8 octet={:#x} with index={}", *it, i);
            return Font::REPLACEMENT_CHAR;
        }
        if ((*it & 0b11'000000) >> 6 != 0b10) {
            SPDLOG_ERROR("Invalid UTF-8 octet=0b{:08b} with index={}", *it, i);
            return Font::REPLACEMENT_CHAR;
        }

        codepoint |= (*it & 0b00'111111) << (6 * (bytesRequired - i - 1));
    }
    if (i + 1 < bytesRequired && it == end) {
        SPDLOG_ERROR("UTF-8 sequence cut off, expected size {}", bytesRequired);
        return Font::REPLACEMENT_CHAR;
    }

    return codepoint;
}
} // namespace utf8
