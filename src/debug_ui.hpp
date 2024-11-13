#pragma once

#include <SDL.h>
#include <cstdint>
#include <glm/ext/vector_float3.hpp>
#include <imgui.h>
#include <imgui_impl_sdl2.h>

class DebugUI {
public:
    struct DebugData {
        uint64_t frameTimeMs;
        glm::vec3& charsPos;
        float& charsScale;
        glm::vec3& cameraPos;
        bool& wireframe;
    };

    DebugUI(SDL_Window* window, SDL_GLContext glContext);
    ~DebugUI();

    void draw(DebugUI::DebugData& data);
    void handleEvent(SDL_Event& event);
    void toggle();

private:
    bool m_Show;
    SDL_Window* m_Window;
};
