#pragma once

#include <glad/glad.h>
// GLFW (include after glad)
#include <GLFW/glfw3.h>

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
