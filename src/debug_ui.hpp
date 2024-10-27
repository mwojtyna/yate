#pragma once

#include <glm/ext/vector_float3.hpp>
#include <imgui.h>
#include <imgui_impl_glfw.h>

class DebugUI {
public:
    struct DebugData {
        double frameTimeMs;
        glm::vec3& charsPos;
        float& charsScale;
        glm::vec3& cameraPos;
        bool& wireframe;
    };

    static void initialize(GLFWwindow* window);
    static void destroy();

    static void draw(DebugUI::DebugData& data);
    static void toggle();

private:
    static ImGuiIO s_IO;
    static bool s_ShowDemoWindow;
};
