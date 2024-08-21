#pragma once

#include "opengl.hpp"

class Application {
public:
    ~Application();
    void start();

    constexpr static GLsizei WIDTH = 800;
    constexpr static GLsizei HEIGHT = 600;
};
