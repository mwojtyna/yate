#pragma once

#include "../debug_ui.hpp"
#include "terminal.hpp"
#include <SDL.h>

class EventHandler {
public:
    EventHandler(SDL_Window* window);
    ~EventHandler();

    void handleEvents(bool& quit, Terminal& terminal, DebugUI& debugUI);

private:
    SDL_Window* m_Window;
};
