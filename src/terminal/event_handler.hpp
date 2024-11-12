#pragma once

#include <SDL2/SDL.h>

class EventHandler {
public:
    EventHandler(SDL_Window* window);
    ~EventHandler();

    void handleEvents(bool& quit);

private:
    SDL_Window* m_Window;
};
