#include "debug_ui.hpp"
#include "application.hpp"
#include <cstdio>
#include <cstdlib>
#include <glm/ext/vector_float3.hpp>
#include <imgui.h>
#include <imgui_impl_opengl3.h>
#include <imgui_impl_sdl2.h>
#include <spdlog/spdlog.h>

DebugUI::DebugUI(SDL_Window* window, SDL_GLContext glContext)
    : m_Window(window) {
    IMGUI_CHECKVERSION();
    ImGui::CreateContext();
    ImGuiIO io = ImGui::GetIO();
    io.Fonts->AddFontDefault();
    io.Fonts->Build();
    ImGui::StyleColorsDark();
    ImGui_ImplSDL2_InitForOpenGL(m_Window, glContext);
    ImGui_ImplOpenGL3_Init();

    SPDLOG_DEBUG("Initialized debug UI");
}

DebugUI::~DebugUI() {
    ImGui_ImplOpenGL3_Shutdown();
    ImGui_ImplSDL2_Shutdown();
    ImGui::DestroyContext();
    SPDLOG_DEBUG("Shutdown debug UI");
}

void DebugUI::draw(DebugUI::DebugData& data) {
    if (!m_Show) {
        return;
    }

    ImGui_ImplOpenGL3_NewFrame();
    ImGui_ImplSDL2_NewFrame();
    ImGui::NewFrame();

    {
        char title[64];
        snprintf(title, 64, "Debug (%f FPS, %lu ms)###DebugWindow",
                 1000.0 / data.frameTimeMs, data.frameTimeMs);

        ImGui::Begin(title);

        ImGui::SeparatorText("Characters");
        ImGui::SliderFloat2("position###charPos", &data.charsPos.x,
                            -Application::WIDTH * 10, Application::WIDTH * 10);
        ImGui::SliderFloat("scale###charScale", &data.charsScale, 0.0f, 50.0f);

        ImGui::SeparatorText("Camera");
        ImGui::SliderFloat2("position###camPos", &data.cameraPos.x,
                            -Application::WIDTH, Application::WIDTH);

        ImGui::Separator();
        ImGui::Checkbox("wireframe", &data.wireframe);

        ImGui::End();
    }

    ImGui::Render();
    ImGui_ImplOpenGL3_RenderDrawData(ImGui::GetDrawData());
}

void DebugUI::handleEvent(SDL_Event& event) {
    ImGui_ImplSDL2_ProcessEvent(&event);
}

void DebugUI::toggle() {
    m_Show = !m_Show;
}
