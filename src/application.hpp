#pragma once

#include "glad/glad.h"
#include "renderer.hpp"

class Application {
public:
    ~Application();
    bool start();

    constexpr static GLsizei WIDTH = 800;
    constexpr static GLsizei HEIGHT = 600;
    constexpr static float ASPECT = (float)WIDTH / HEIGHT;

private:
    Renderer m_Renderer;
};
