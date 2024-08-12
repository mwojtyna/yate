#include "glad/glad.h"
// GLFW (include after glad)
#include "GLFW/glfw3.h"

#include "application.hpp"
#include "debug_ui.hpp"
#include "glm/ext/matrix_float4x4.hpp"
#include "glm/ext/matrix_transform.hpp"
#include "glm/ext/vector_float3.hpp"
#include "rendering/program.hpp"
#include "rendering/renderer.hpp"
#include "rendering/text_renderer.hpp"
#include "shaders/fragment.frag.hpp"
#include "shaders/vertex.vert.hpp"
#include "spdlog/cfg/env.h"
#include "spdlog/spdlog.h"

void Application::start() {
    spdlog::cfg::load_env_levels();
    SPDLOG_DEBUG("GLFW version: {}", glfwGetVersionString());

    glfwInit();
    glfwWindowHint(GLFW_CONTEXT_VERSION_MAJOR, 3);
    glfwWindowHint(GLFW_CONTEXT_VERSION_MINOR, 3);
    glfwWindowHint(GLFW_OPENGL_PROFILE, GLFW_OPENGL_CORE_PROFILE);

    GLFWwindow* window = glfwCreateWindow(
        Application::WIDTH, Application::HEIGHT, "yate", nullptr, nullptr);
    if (window == nullptr) {
        const char* error;
        glfwGetError(&error);
        SPDLOG_ERROR("Failed to create window: {}", error);
        assert(false);
    }
    glfwMakeContextCurrent(window);

    Renderer::initialize();
    Renderer::setBgColor(glm::vec3(0.2f, 0.3f, 0.3f));
    Renderer::setWireframe(false);

    TextRenderer::initialize();
    TextRenderer::loadFont("/usr/share/fonts/TTF/JetBrainsMono-Regular.ttf");
    TextRenderer::renderChar('A');

    Program program = ProgramBuilder()
                          .loadShader(vertexShader, GL_VERTEX_SHADER)
                          .loadShader(fragmentShader, GL_FRAGMENT_SHADER)
                          .build();

    DebugUI::initialize(window);
    double prevTime = glfwGetTime();
    glm::vec3 charPos(0.0f), cameraPos(0.0f);
    float cameraScale = 1.0f;
    auto debugData = DebugUI::DebugData{
        .frameTimeMs = 0,
        .charPos = charPos,
        .cameraPos = cameraPos,
        .cameraScale = cameraScale,
    };

    SPDLOG_INFO("Application started");
    while (!glfwWindowShouldClose(window)) {
        glm::mat4 transform = glm::translate(glm::mat4(1.0f), charPos);
        Renderer::setViewMat(
            glm::scale(glm::translate(glm::mat4(1.0f), cameraPos),
                       glm::vec3(cameraScale)));
        Renderer::draw("Hello world!", transform, program);

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
    TextRenderer::destroy();
    Renderer::destroy();
    glfwTerminate();
}
