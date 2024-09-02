#pragma once

#include "terminal/terminal.hpp"
#include <memory>
#include <thread>

class Application {
public:
    ~Application();
    void start();

    Terminal m_Terminal;
    std::unique_ptr<std::thread> m_TerminalThread;

    constexpr static int WIDTH = 800;
    constexpr static int HEIGHT = 600;
};
