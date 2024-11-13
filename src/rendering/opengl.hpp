#pragma once

#include <assert.h>
#include <glad/glad.h>

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
