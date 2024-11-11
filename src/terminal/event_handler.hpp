#pragma once

#include <SDL3/SDL_video.h>

class EventHandler {
public:
    EventHandler(SDL_Window* window);
    void handleEvents(bool& quit);

private:
    SDL_Window* m_Window;
};
