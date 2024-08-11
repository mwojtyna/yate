#include "debug_ui.hpp"
#include "GLFW/glfw3.h"
#include "glm/ext/vector_float3.hpp"
#include "imgui.h"
#include "imgui_impl_glfw.h"
#include "imgui_impl_opengl3.h"
#include "spdlog/spdlog.h"
#include <cstdio>
#include <cstdlib>
#include <memory>

DebugUI::DebugUI(GLFWwindow* window) {
    IMGUI_CHECKVERSION();
    ImGui::CreateContext();
    m_IO = std::make_unique<ImGuiIO>(ImGui::GetIO());
    m_IO->ConfigFlags |=
        ImGuiConfigFlags_NavEnableKeyboard; // Enable Keyboard Controls
    m_IO->Fonts->AddFontDefault();
    m_IO->Fonts->Build();
    ImGui::StyleColorsDark();
    ImGui_ImplGlfw_InitForOpenGL(window, true);
    ImGui_ImplOpenGL3_Init();

    SPDLOG_DEBUG("Initialized debug UI");
}

DebugUI::~DebugUI() {
    ImGui_ImplOpenGL3_Shutdown();
    ImGui_ImplGlfw_Shutdown();
    ImGui::DestroyContext();
    SPDLOG_DEBUG("Destroyed debug UI");
}

void DebugUI::draw(DebugUI::DebugData& data) {
    ImGui_ImplOpenGL3_NewFrame();
    ImGui_ImplGlfw_NewFrame();
    ImGui::NewFrame();

    {
        char title[64];
        sprintf(title, "Debug (%f FPS, %f ms)###DebugWindow",
                1000 / data.frameTimeMs, data.frameTimeMs);

        ImGui::Begin(title);
        ImGui::SliderFloat2("position", &data.translation.x, -2.0f, 2.0f);
        ImGui::End();
    }

    ImGui::Render();
    ImGui_ImplOpenGL3_RenderDrawData(ImGui::GetDrawData());
}
