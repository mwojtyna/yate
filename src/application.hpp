#pragma once

// For GLFWwindow
#include "rendering/opengl.hpp"

#include <memory>
#include <thread>

class Application {
public:
    void start();
    ~Application();
    GLFWwindow* getWindow();

    constexpr static int WIDTH = 800;
    constexpr static int HEIGHT = 600;

private:
    GLFWwindow* m_Window;
    std::unique_ptr<std::thread> m_TerminalThread;
};
