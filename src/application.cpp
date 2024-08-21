#include "application.hpp"
#include "debug_ui.hpp"
#include "rendering/font.hpp"
#include "rendering/program.hpp"
#include "rendering/renderer.hpp"
#include "shaders/text.frag.hpp"
#include "shaders/text.vert.hpp"
#include <glm/ext/matrix_float4x4.hpp>
#include <glm/ext/matrix_transform.hpp>
#include <glm/ext/vector_float3.hpp>
#include <spdlog/cfg/env.h>
#include <spdlog/spdlog.h>

void Application::start() {
    spdlog::cfg::load_env_levels();
    SPDLOG_INFO("GLFW version: {}", glfwGetVersionString());

    glfwInit();
    glfwWindowHint(GLFW_CONTEXT_VERSION_MAJOR, 3);
    glfwWindowHint(GLFW_CONTEXT_VERSION_MINOR, 3);
    glfwWindowHint(GLFW_OPENGL_PROFILE, GLFW_OPENGL_CORE_PROFILE);

    GLFWwindow* window = glfwCreateWindow(
        Application::WIDTH, Application::HEIGHT, "yate", nullptr, nullptr);
    if (window == nullptr) {
        const char* error;
        glfwGetError(&error);
        FATAL("Failed to create window: {}", error);
    }
    glfwMakeContextCurrent(window);

    // TODO: Initialize terminal

    Renderer::initialize();
    Renderer::setBgColor(glm::vec3(0.10f, 0.11f, 0.15f));
    Renderer::setWireframe(false);

    Font font("/usr/share/fonts/TTF/JetBrainsMonoNerdFont-Regular.ttf", 64);
    font.createAtlas();

    Program program(textVertexShader, textFragmentShader);

    glm::vec3 charsPos(0.0f), cameraPos(0.0f);
    float charsScale = 1.0f;
    double prevTime = glfwGetTime();
    auto debugData = DebugUI::DebugData{
        .frameTimeMs = 0,
        .charsPos = &charsPos,
        .charsScale = &charsScale,
        .cameraPos = &cameraPos,
    };
    DebugUI::initialize(window);

    Codes codes = {72,  101, 108, 108, 111, 32, 119,
                   111, 103, 114, 108, 100, 33};
    SPDLOG_INFO("Application started");
    while (!glfwWindowShouldClose(window)) {
        // TODO: Parse terminal codes

        glm::mat4 transform = glm::scale(
            glm::translate(glm::mat4(1.0f), charsPos), glm::vec3(charsScale));
        Renderer::setViewMat(glm::translate(glm::mat4(1.0f), cameraPos));
        Renderer::drawText(codes, font, transform, program);

        debugData.frameTimeMs = (glfwGetTime() - prevTime) * 1000;
        prevTime = glfwGetTime();
        DebugUI::draw(debugData);

        glfwSwapBuffers(window);
        glfwPollEvents();
    }
}

Application::~Application() {
    SPDLOG_INFO("Application exiting");
    DebugUI::destroy();
    Renderer::destroy();
    glfwTerminate();
}
