#include "event_handler.hpp"
#include "../debug_ui.hpp"
#include "codes.hpp"
#include "csi_idents.hpp"
#include "nav_keys.hpp"
#include "terminal.hpp"
#include <SDL3/SDL_events.h>
#include <SDL3/SDL_keyboard.h>
#include <SDL3/SDL_keycode.h>
#include <SDL3/SDL_video.h>

EventHandler::EventHandler(SDL_Window* window) : m_Window(window) {
    SDL_StartTextInput(m_Window);
}

void EventHandler::handleEvents(bool& quit) {
    SDL_Event event;
    while (SDL_PollEvent(&event)) {
        switch (event.type) {
        case SDL_EVENT_QUIT: {
            quit = true;
            return;
        }
        case SDL_EVENT_TEXT_INPUT: {
            const char* text = event.text.text;
            std::vector<uint8_t> buf;
            for (size_t i = 0; text[i] != '\0'; i++) {
                buf.push_back(text[i]);
            }
            Terminal::write(std::move(buf));
            break;
        }
        case SDL_EVENT_KEY_DOWN: {
            switch (event.key.key) {
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
                DebugUI::toggle();
                break;
            }
#endif
            }

            switch (event.key.mod) {
            case SDL_KMOD_RCTRL:
            case SDL_KMOD_LCTRL: {
                if (std::isalpha(event.key.key)) {
                    Terminal::write(
                        {static_cast<uint8_t>(1 + event.key.key - 'a')});
                }
                break;
            }
            }
            break;
        }
        }
    }
}
