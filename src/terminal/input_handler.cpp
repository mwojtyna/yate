#include "input_handler.hpp"
#include "../debug_ui.hpp"
#include "codes.hpp"
#include "csi_idents.hpp"
#include "nav_keys.hpp"
#include "terminal.hpp"

InputHandler::InputHandler(GLFWwindow* window) : m_Window(window) {}

void InputHandler::setupHandlers() {
    glfwSetCharCallback(m_Window, [](GLFWwindow* window, uint32_t codepoint) {
        // TODO: UTF-8
        Terminal::write({(uint8_t)codepoint});
    });
    glfwSetKeyCallback(m_Window, [](GLFWwindow* window, int key, int scanCode,
                                    int action, int mods) {
        if (action != GLFW_PRESS) {
            return;
        }

        // Handle normal keys
        switch (key) {
        case GLFW_KEY_BACKSPACE: {
            Terminal::write({c0::BS});
            break;
        }
        case GLFW_KEY_ENTER: {
            Terminal::write({c0::LF});
            break;
        }
        case GLFW_KEY_RIGHT: {
            Terminal::write(csiidents::CUF.data(std::nullopt));
            break;
        }
        case GLFW_KEY_LEFT: {
            Terminal::write(csiidents::CUB.data(std::nullopt));
            break;
        }
        case GLFW_KEY_UP: {
            Terminal::write(csiidents::CUU.data(std::nullopt));
            break;
        }
        case GLFW_KEY_DOWN: {
            Terminal::write(csiidents::CUD.data(std::nullopt));
            break;
        }
        case GLFW_KEY_HOME: {
            Terminal::write(csiidents::Home());
            break;
        }
        case GLFW_KEY_END: {
            Terminal::write(csiidents::End());
            break;
        }
        case GLFW_KEY_PAGE_UP: {
            Terminal::write(csiidents::PageUp());
            break;
        }
        case GLFW_KEY_PAGE_DOWN: {
            Terminal::write(csiidents::PageDown());
            break;
        }
        case GLFW_KEY_INSERT: {
            Terminal::write(csiidents::Insert());
            break;
        }
        case GLFW_KEY_DELETE: {
            Terminal::write(csiidents::Delete());
            break;
        }
        case GLFW_KEY_TAB: {
            Terminal::write({c0::HT});
            break;
        }

#ifndef NDEBUG
        case GLFW_KEY_F12: {
            DebugUI::toggle();
            break;
        }
#endif
        }

        switch (mods) {
        case GLFW_MOD_CONTROL: {
            if (std::isalpha(key)) {
                Terminal::write({static_cast<uint8_t>(0x1 + key - 'A')});
            }
            break;
        }
        }
    });
}
