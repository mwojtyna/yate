#pragma once

#include <SDL3/SDL_video.h>
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
    std::unique_ptr<std::thread> m_TerminalThread;
};
