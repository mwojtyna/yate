#include "application.hpp"
#include "debug_ui.hpp"
#include "rendering/font.hpp"
#include "rendering/program.hpp"
#include "rendering/renderer.hpp"
#include "shaders/text.frag.hpp"
#include "shaders/text.vert.hpp"
#include "terminal/input_handler.hpp"
#include "terminal/parser.hpp"
#include "terminal/parser_setup.hpp"
#include "terminal/terminal.hpp"
#include "terminal/thread_safe_queue.hpp"
#include "utils.hpp"
#include <cassert>
#include <cstdint>
#include <glm/ext/matrix_float4x4.hpp>
#include <glm/ext/matrix_transform.hpp>
#include <glm/ext/vector_float3.hpp>
#include <memory>
#include <spdlog/cfg/env.h>
#include <spdlog/spdlog.h>
#include <string>
#include <unordered_set>

void Application::start() {
    spdlog::cfg::load_env_levels();

    SPDLOG_INFO("GLFW version: {}", glfwGetVersionString());
    glfwInit();
    glfwWindowHint(GLFW_CONTEXT_VERSION_MAJOR, 3);
    glfwWindowHint(GLFW_CONTEXT_VERSION_MINOR, 3);
    glfwWindowHint(GLFW_OPENGL_PROFILE, GLFW_OPENGL_CORE_PROFILE);

    m_Window = glfwCreateWindow(Application::WIDTH, Application::HEIGHT, "yate",
                                nullptr, nullptr);

    if (m_Window == nullptr) {
        const char* error;
        glfwGetError(&error);
        FATAL("Failed to create window: {}", error);
    }
    glfwMakeContextCurrent(m_Window);

    InputHandler inputHandler(m_Window);
    inputHandler.setupHandlers();

    Renderer::initialize();
    Renderer::setBgColor(glm::vec3(0.10f, 0.11f, 0.15f));
    Font font("/usr/share/fonts/TTF/JetBrainsMonoNerdFont-Regular.ttf", 16);

    ThreadSafeQueue<codepoint_t> atlasQueue;
    Terminal::open(Application::WIDTH, Application::HEIGHT);
    m_TerminalThread = std::make_unique<std::thread>([this, &atlasQueue,
                                                      &font]() {
        Parser parser = parser_setup(m_Window);
        while (!Terminal::shouldClose()) {
            try {
                std::vector<uint8_t> rawCodes = Terminal::read();
                SPDLOG_DEBUG("Read from terminal:");
                HEXDUMP(rawCodes.data(), rawCodes.size());

                std::vector<codepoint_t> codepoints =
                    parser.parseAndModifyTermBuf(rawCodes);
                if (codepoints.empty()) {
                    continue;
                }
                for (const codepoint_t c : codepoints) {
                    atlasQueue.push(c);
                }

                Terminal::getBuf([&font](const TerminalBuf& termBuf) {
                    // TODO: Don't recalculate all
                    Renderer::makeTextMesh(termBuf.getRows(), font);
                });

            } catch (TerminalReadException e) {
                if (!Terminal::shouldClose()) {
                    SPDLOG_ERROR("Failed reading from terminal: {}", e.what());
                }
            }
        }
        SPDLOG_DEBUG("Terminal thread finished");
    });

    Program program(textVertexShader, textFragmentShader);

    glm::vec3 charsPos(glm::round(-font.getMetrics().max_advance +
                                  font.getMetrics().max_advance * 0.25),
                       glm::round(-font.getMetrics().height), 0);
    float charsScale = 1.0f;
    glm::vec3 cameraPos(0);
    bool wireframe = false;
    double prevTime = glfwGetTime();
    auto debugData = DebugUI::DebugData{.frameTimeMs = 0,
                                        .charsPos = &charsPos,
                                        .charsScale = &charsScale,
                                        .cameraPos = &cameraPos,
                                        .wireframe = &wireframe};
    DebugUI::initialize(m_Window);

    SPDLOG_INFO("Application started");

    std::unordered_set<codepoint_t> codepoints(atlasQueue.size());
    while (!glfwWindowShouldClose(m_Window)) {
        Renderer::clear();
        Renderer::setWireframe(*debugData.wireframe);

        glm::mat4 transform = glm::scale(
            glm::translate(glm::mat4(1.0f), charsPos), glm::vec3(charsScale));
        Renderer::setViewMat(glm::translate(glm::mat4(1.0f), cameraPos));

        // When new terminal data appears, update font atlas for new glyphs
        if (codepoint_t tmp; atlasQueue.pop(tmp)) {
            codepoints.insert(tmp);
        }
        font.updateAtlas(codepoints);
        codepoints.clear();

        Renderer::drawText(transform, program);

        debugData.frameTimeMs = (glfwGetTime() - prevTime) * 1000;
        prevTime = glfwGetTime();
        DebugUI::draw(debugData);

        glfwSwapBuffers(m_Window);
        glfwPollEvents();
    }
}

Application::~Application() {
    SPDLOG_INFO("Application exiting");
    DebugUI::destroy();
    Renderer::destroy();
    // FIX: Doesn't work on macos, because read() doesn't error for some reason
    Terminal::close();
    assert(m_TerminalThread->joinable());
    m_TerminalThread->join();
    glfwTerminate();
}

GLFWwindow* Application::getWindow() {
    return m_Window;
}
