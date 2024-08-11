#pragma once

#include "glm/ext/vector_float3.hpp"
#include "imgui.h"
#include "imgui_impl_glfw.h"
#include <memory>

class DebugUI {
public:
    DebugUI(GLFWwindow* window);
    ~DebugUI();

    void draw(glm::vec3& transform);

private:
    std::unique_ptr<ImGuiIO> m_IO;
    bool m_ShowDemoWindow = false;
};
