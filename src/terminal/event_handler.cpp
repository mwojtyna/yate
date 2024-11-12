#include "event_handler.hpp"
#include "codes.hpp"
#include "csi_idents.hpp"
#include "nav_keys.hpp"
#include "terminal.hpp"

EventHandler::EventHandler(SDL_Window* window) : m_Window(window) {
    SDL_StartTextInput();
}
EventHandler::~EventHandler() {
    SDL_StopTextInput();
}

void EventHandler::handleEvents(bool& quit, DebugUI& debugUI) {
    SDL_Event event;
    while (SDL_PollEvent(&event)) {
        debugUI.handleEvent(event);

        switch (event.type) {
        case SDL_QUIT: {
            quit = true;
            return;
        }
        case SDL_TEXTINPUT: {
            const char* text = event.text.text;
            std::vector<uint8_t> buf;
            for (size_t i = 0; text[i] != '\0'; i++) {
                buf.push_back(text[i]);
            }
            Terminal::write(std::move(buf));
            break;
        }
        case SDL_KEYDOWN: {
            SDL_Keysym key = event.key.keysym;

            switch (key.sym) {
            case SDLK_BACKSPACE: {
                Terminal::write({c0::BS});
                break;
            }
            case SDLK_RETURN: {
                Terminal::write({c0::LF});
                break;
            }
            case SDLK_RIGHT: {
                Terminal::write(csiidents::CUF.data(std::nullopt));
                break;
            }
            case SDLK_LEFT: {
                Terminal::write(csiidents::CUB.data(std::nullopt));
                break;
            }
            case SDLK_UP: {
                Terminal::write(csiidents::CUU.data(std::nullopt));
                break;
            }
            case SDLK_DOWN: {
                Terminal::write(csiidents::CUD.data(std::nullopt));
                break;
            }
            case SDLK_HOME: {
                Terminal::write(csiidents::Home());
                break;
            }
            case SDLK_END: {
                Terminal::write(csiidents::End());
                break;
            }
            case SDLK_PAGEUP: {
                Terminal::write(csiidents::PageUp());
                break;
            }
            case SDLK_PAGEDOWN: {
                Terminal::write(csiidents::PageDown());
                break;
            }
            case SDLK_INSERT: {
                Terminal::write(csiidents::Insert());
                break;
            }
            case SDLK_DELETE: {
                Terminal::write(csiidents::Delete());
                break;
            }
            case SDLK_TAB: {
                Terminal::write({c0::HT});
                break;
            }

#ifndef NDEBUG
            case SDLK_F12: {
                debugUI.toggle();
                break;
            }
#endif
            }

            // Have to stop text input when Ctrl is held down to allow for repeating Ctrl-C for example
            if (key.mod & KMOD_CTRL) {
                SDL_StopTextInput();
                // Have to check range otherwise segfault if big number
                if (key.sym <= 127 && std::isalpha(key.sym)) {
                    Terminal::write({static_cast<uint8_t>(1 + key.sym - 'a')});
                }
                SDL_StartTextInput();
            }
            break;
        }
        }
    }
}
