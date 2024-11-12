#include "debug_ui.hpp"
#include "application.hpp"
#include <cstdio>
#include <cstdlib>
#include <glm/ext/vector_float3.hpp>
#include <imgui.h>
#include <imgui_impl_opengl3.h>
#include <imgui_impl_sdl2.h>
#include <spdlog/spdlog.h>

ImGuiIO DebugUI::s_IO;
bool DebugUI::s_ShowDemoWindow = false;

void DebugUI::initialize(SDL_Window* window, SDL_GLContext glContext) {
    IMGUI_CHECKVERSION();
    ImGui::CreateContext();
    s_IO = ImGui::GetIO();
    s_IO.Fonts->AddFontDefault();
    s_IO.Fonts->Build();
    ImGui::StyleColorsDark();
    ImGui_ImplSDL2_InitForOpenGL(window, glContext);
    ImGui_ImplOpenGL3_Init();

    SPDLOG_DEBUG("Initialized debug UI");
}

void DebugUI::destroy() {
    ImGui_ImplOpenGL3_Shutdown();
    ImGui_ImplSDL2_Shutdown();
    ImGui::DestroyContext();
    SPDLOG_DEBUG("Shutdown debug UI");
}

void DebugUI::draw(DebugUI::DebugData& data) {
    if (!s_ShowDemoWindow) {
        return;
    }

    ImGui_ImplOpenGL3_NewFrame();
    ImGui_ImplSDL2_NewFrame();
    ImGui::NewFrame();

    {
        char title[64];
        snprintf(title, 64, "Debug (%f FPS, %llu ms)###DebugWindow",
                 1000.0 / data.frameTimeMs, data.frameTimeMs);

        ImGui::Begin(title);

        ImGui::SeparatorText("Characters");
        ImGui::SliderFloat2("position###charPos", &data.charsPos.x,
                            -Application::WIDTH * 2, Application::WIDTH * 2);
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

void DebugUI::toggle() {
    s_ShowDemoWindow = !s_ShowDemoWindow;
}
