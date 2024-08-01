#pragma once

#include <cstdint>

class Application {
  public:
    bool start();
    ~Application();

  private:
    const uint32_t WIDTH = 800;
    const uint32_t HEIGHT = 600;
    const char *TITLE = "yate";
};
