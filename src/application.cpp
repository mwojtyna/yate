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
#include <unordered_set>

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
    auto ascii = std::unordered_set<Codepoint>();
    for (Codepoint c = 0x20; c <= 0x7e; c++) {
        ascii.insert(c);
    }
    ascii.insert(Font::REPLACEMENT_CHAR); // replacement character �
    font.updateAtlas(ascii);

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

    // TODO: Parse terminal codes
    std::vector<Codepoint> terminalCodes = {
        90,  97,  380, 243, 322, 263, 32,  103, 281, 347, 108, 261, 32,  106,
        97,  378, 324, 33,  10,  84,  104, 101, 32,  113, 117, 105, 99,  107,
        32,  98,  114, 111, 119, 110, 32,  102, 111, 120, 32,  106, 117, 109,
        112, 115, 32,  111, 118, 101, 114, 32,  116, 104, 101, 32,  108, 97,
        122, 121, 32,  100, 111, 103, 46,  10,  9,   113, 33,  64,  35,  36,
        37,  94,  38,  42,  40,  41,  45,  95,  61,  43,  91,  123, 93,  125,
        59,  58,  39,  34,  44,  60,  46,  62,  47,  63};

    // TODO: If terminalCodes changed from last frame, update codepoints and add to atlas
    auto codepoints = std::unordered_set<Codepoint>(terminalCodes.begin(),
                                                    terminalCodes.end());
    font.updateAtlas(codepoints);

    SPDLOG_INFO("Application started");
    while (!glfwWindowShouldClose(window)) {
        glm::mat4 transform = glm::scale(
            glm::translate(glm::mat4(1.0f), charsPos), glm::vec3(charsScale));
        Renderer::setViewMat(glm::translate(glm::mat4(1.0f), cameraPos));
        Renderer::drawText(terminalCodes, font, transform, program);

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
