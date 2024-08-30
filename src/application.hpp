#pragma once

#include <cstddef>

class Application {
public:
    ~Application();
    void start();

    constexpr static size_t WIDTH = 800;
    constexpr static size_t HEIGHT = 600;
};
