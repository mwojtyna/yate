#pragma once

#include <cstdlib>
#include <spdlog/common.h>
#include <spdlog/spdlog.h>
#include <string>

#define FATAL(...)                                                             \
    SPDLOG_ERROR(__VA_ARGS__);                                                 \
    std::exit(EXIT_FAILURE);

#if SPDLOG_ACTIVE_LEVEL <= SPDLOG_LEVEL_DEBUG
#define HEXDUMP(ptr, len) (hexdump(ptr, len))
#else
#define HEXDUMP(ptr, len)
#endif

void hexdump(void* ptr, size_t len, int minLogLevel = SPDLOG_LEVEL_DEBUG);

// https://stackoverflow.com/a/19195373/9854703
template <class T>
inline void hashCombine(std::size_t& s, const T& v) {
    std::hash<T> h;
    s ^= h(v) + 0x9e3779b9 + (s << 6) + (s >> 2);
}

template <typename T>
std::string optionalToString(std::optional<T>& optional) {
    if (optional.has_value()) {
        return std::to_string(optional.value());
    } else {
        return std::string("nullopt");
    }
}
template <>
inline std::string optionalToString(std::optional<char>& optional) {
    if (optional.has_value()) {
        return std::string{optional.value()};
    } else {
        return std::string("nullopt");
    }
}
