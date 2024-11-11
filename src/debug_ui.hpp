#pragma once

#include <SDL3/SDL_video.h>
#include <cstdint>
#include <glm/ext/vector_float3.hpp>
#include <imgui.h>
#include <imgui_impl_sdl3.h>

class DebugUI {
public:
    struct DebugData {
        uint64_t frameTimeMs;
        glm::vec3& charsPos;
        float& charsScale;
        glm::vec3& cameraPos;
        bool& wireframe;
    };

    static void initialize(SDL_Window* window, SDL_GLContext glContext);
    static void destroy();

    static void draw(DebugUI::DebugData& data);
    static void toggle();

private:
    static ImGuiIO s_IO;
    static bool s_ShowDemoWindow;
};
