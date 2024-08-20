#pragma once

#include <assert.h>

#ifdef NDEBUG
#define glCall(x) x;
#else
#define glCall(x)                                                              \
    clearError();                                                              \
    x;                                                                         \
    assert(checkError());
#endif

#define FATAL(...)                                                             \
    SPDLOG_ERROR(__VA_ARGS__);                                                 \
    std::exit(1);

bool checkError();
void clearError();
