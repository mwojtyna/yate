#include "debug_ui.hpp"
#include "application.hpp"
#include "glm/ext/vector_float3.hpp"
#include "imgui.h"
#include "imgui_impl_glfw.h"
#include "imgui_impl_opengl3.h"
#include "spdlog/spdlog.h"
#include <cstdio>
#include <cstdlib>

ImGuiIO DebugUI::m_IO;
bool DebugUI::m_ShowDemoWindow = false;

void DebugUI::initialize(GLFWwindow* window) {
    IMGUI_CHECKVERSION();
    ImGui::CreateContext();
    m_IO = ImGui::GetIO();
    m_IO.ConfigFlags |=
        ImGuiConfigFlags_NavEnableKeyboard; // Enable Keyboard Controls
    m_IO.Fonts->AddFontDefault();
    m_IO.Fonts->Build();
    ImGui::StyleColorsDark();
    ImGui_ImplGlfw_InitForOpenGL(window, true);
    ImGui_ImplOpenGL3_Init();

    SPDLOG_DEBUG("Initialized debug UI");
}

void DebugUI::destroy() {
    ImGui_ImplOpenGL3_Shutdown();
    ImGui_ImplGlfw_Shutdown();
    ImGui::DestroyContext();
    SPDLOG_DEBUG("Shutdown debug UI");
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

        ImGui::SeparatorText("Characters");
        ImGui::SliderFloat2("position###charPos", &data.charsPos->x,
                            -Application::WIDTH * 2, Application::WIDTH * 2);
        ImGui::SliderFloat("scale###charScale", data.charsScale, 0.0f, 50.0f);

        ImGui::SeparatorText("Camera");
        ImGui::SliderFloat2("position###camPos", &data.cameraPos->x,
                            -Application::WIDTH, Application::WIDTH);

        ImGui::End();
    }

    ImGui::Render();
    ImGui_ImplOpenGL3_RenderDrawData(ImGui::GetDrawData());
}
