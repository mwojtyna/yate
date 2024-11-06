#pragma once

#include <cstdint>
#include <cstdlib>
#include <spdlog/common.h>
#include <spdlog/spdlog.h>
#include <string>

#define FATAL(...)                                                             \
    SPDLOG_ERROR(__VA_ARGS__);                                                 \
    std::exit(EXIT_FAILURE)

#if SPDLOG_ACTIVE_LEVEL <= SPDLOG_LEVEL_DEBUG
#define HEXDUMP(ptr, len) (hexdump(ptr, len))
#else
#define HEXDUMP(ptr, len)
#endif

// https://stackoverflow.com/a/29865/9854703
template <typename T = uint8_t>
void hexdump(T* ptr, size_t len, int minLogLevel = SPDLOG_LEVEL_DEBUG) {
    if (spdlog::get_level() > minLogLevel) {
        return;
    }

    T* buf = (T*)ptr;
    size_t i, j;
    printf("len: %zu\n", len);
    for (i = 0; i < len; i += 16) {
        printf("%06zx: ", i);

        for (j = 0; j < 16; j++) {
            if (i + j < len) {
                printf("%02x ", buf[i + j]);
            } else {
                printf("   ");
            }
        }
        printf(" ");

        for (j = 0; j < 16; j++) {
            if (i + j < len) {
                printf("%c", isprint(buf[i + j]) ? buf[i + j] : '.');
            }
        }
        printf("\n");
    }
}

// https://stackoverflow.com/a/19195373/9854703
template <class T>
inline void hashCombine(std::size_t& s, const T& v) {
    std::hash<T> h;
    s ^= h(v) + 0x9e3779b9 + (s << 6) + (s >> 2);
}

template <typename T>
std::string optionalToString(const std::optional<T>& optional) {
    if (optional.has_value()) {
        return std::to_string(optional.value());
    } else {
        return std::string("nullopt");
    }
}
template <>
inline std::string optionalToString(const std::optional<char>& optional) {
    if (optional.has_value()) {
        return std::string{optional.value()};
    } else {
        return std::string("nullopt");
    }
}

template <typename T>
std::string vectorToString(const std::vector<T>& vec) {
    std::string out = "[";
    for (size_t i = 0; i < vec.size(); i++) {
        out += std::to_string(vec[i]);
        if (i < vec.size() - 1) {
            out += ",";
        }
    }
    out += "]";
    return out;
}
template <>
inline std::string vectorToString(const std::vector<char>& vec) {
    std::string out = "[";
    for (size_t i = 0; i < vec.size(); i++) {
        out += vec[i];
        if (i < vec.size() - 1) {
            out += ",";
        }
    }
    out += "]";
    return out;
}
template <>
inline std::string vectorToString(const std::vector<std::string>& vec) {
    std::string out = "[";
    for (size_t i = 0; i < vec.size(); i++) {
        out += vec[i];
        if (i < vec.size() - 1) {
            out += ",";
        }
    }
    out += "]";
    return out;
}
