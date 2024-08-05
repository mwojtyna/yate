#pragma once

#include "glad/glad.h"

class Application {
  public:
    bool start();
    ~Application();

  private:
    const static GLuint WIDTH = 800;
    const static GLuint HEIGHT = 600;
};
