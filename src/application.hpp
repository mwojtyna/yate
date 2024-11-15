#pragma once

#include "terminal/terminal.hpp"
#include <SDL.h>
#include <memory>
#include <thread>

class Application {
public:
    void start();
    ~Application();
    SDL_Window* getWindow() const;

    constexpr static int WIDTH = 1200;
    constexpr static int HEIGHT = 840;

private:
    SDL_Window* m_Window;
    Terminal m_Terminal;
    std::unique_ptr<std::thread> m_TerminalThread;
};
