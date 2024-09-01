#pragma once

class Application {
public:
    ~Application();
    void start();

    constexpr static int WIDTH = 800;
    constexpr static int HEIGHT = 600;
};
