#pragma once

#include "glad/glad.h"

class Application {
public:
    ~Application();
    void start();

    constexpr static GLsizei WIDTH = 800;
    constexpr static GLsizei HEIGHT = 600;
};
