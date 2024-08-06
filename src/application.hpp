#pragma once

#include "glad/glad.h"
#include "renderer.hpp"

class Application {
  public:
    ~Application();
    bool start() const;

  private:
    const static GLuint WIDTH = 800;
    const static GLuint HEIGHT = 600;

    Renderer m_Renderer;
};
