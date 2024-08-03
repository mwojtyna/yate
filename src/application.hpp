#pragma once

#include "renderer.hpp"
#include <cstdint>

class Application {
  public:
    bool start();
    ~Application();

  private:
    const static uint32_t WIDTH = 800;
    const static uint32_t HEIGHT = 600;

    Renderer m_Renderer;
};
