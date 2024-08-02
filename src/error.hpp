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

bool checkError();
void clearError();
