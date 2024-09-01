#pragma once

#include <cstdlib>

#define FATAL(...)                                                             \
    SPDLOG_ERROR(__VA_ARGS__);                                                 \
    std::exit(EXIT_FAILURE);

void hexdump(void* ptr, size_t len);
