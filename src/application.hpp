#pragma once

// For GLFWwindow
#include "rendering/opengl.hpp"

#include "terminal/terminal.hpp"
#include <memory>
#include <thread>

class Application {
public:
    ~Application();
    void start();

    constexpr static int WIDTH = 800;
    constexpr static int HEIGHT = 600;

private:
    Terminal m_Terminal;
    std::unique_ptr<std::thread> m_TerminalThread;
    GLFWwindow* m_Window;
};
