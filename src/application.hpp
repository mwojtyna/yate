#pragma once

#include "glad/glad.h"
#include "renderer.hpp"

class Application {
  public:
    bool start();
    ~Application();

  private:
    const static GLuint WIDTH = 800;
    const static GLuint HEIGHT = 600;

    Renderer m_Renderer;
};
