#pragma once

#include "glm/ext/vector_float3.hpp"
#include "imgui.h"
#include "imgui_impl_glfw.h"

class DebugUI {
public:
    struct DebugData {
        double frameTimeMs;
        glm::vec3& charPos;
        glm::vec3& cameraPos;
        float& cameraScale;
    };

    static void initialize(GLFWwindow* window);
    static void destroy();

    static void draw(DebugUI::DebugData& data);

private:
    static ImGuiIO m_IO;
    static bool m_ShowDemoWindow;
};
