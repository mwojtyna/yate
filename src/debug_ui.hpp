#pragma once

#include "glm/ext/vector_float3.hpp"
#include "imgui.h"
#include "imgui_impl_glfw.h"
#include <memory>

class DebugUI {
public:
    struct DebugData {
        double frameTimeMs;
        glm::vec3& translation;
    };

    DebugUI(GLFWwindow* window);
    ~DebugUI();

    void draw(DebugUI::DebugData& data);

private:
    std::unique_ptr<ImGuiIO> m_IO;
    bool m_ShowDemoWindow = false;
};
