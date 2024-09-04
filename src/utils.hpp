#pragma once

#include "spdlog/common.h"
#include <cstdlib>

#define FATAL(...)                                                             \
    SPDLOG_ERROR(__VA_ARGS__);                                                 \
    std::exit(EXIT_FAILURE);

#if SPDLOG_ACTIVE_LEVEL <= SPDLOG_LEVEL_DEBUG
#define HEXDUMP(ptr, len) (hexdump(ptr, len))
#else
#define HEXDUMP(ptr, len)
#endif

void hexdump(void* ptr, size_t len, int minLogLevel = SPDLOG_LEVEL_DEBUG);
