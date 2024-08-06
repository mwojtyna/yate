#pragma once

#include "glad/glad.h"
#include "renderer.hpp"

class Application {
  public:
    ~Application();
    bool start() const;

  private:
    constexpr static GLuint WIDTH = 800;
    constexpr static GLuint HEIGHT = 600;

    Renderer m_Renderer;
};
