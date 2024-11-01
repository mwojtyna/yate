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

    Renderer renderer;
    renderer.initialize();
    renderer.setBgColor(glm::vec3(0.10f, 0.11f, 0.15f));
    Program program(textVertexShader, textFragmentShader);
    Font font("/usr/share/fonts/TTF/JetBrainsMonoNerdFont-Regular.ttf", 16);
    DebugUI::initialize(m_Window);
    InputHandler inputHandler(m_Window);
    inputHandler.setupHandlers();

    ThreadSafeQueue<codepoint_t> atlasQueue;
    Terminal::open(Application::WIDTH, Application::HEIGHT);
    m_TerminalThread = std::make_unique<std::thread>([this, &atlasQueue, &font,
                                                      &renderer]() {
        Parser parser = parser_setup(m_Window);
        while (!Terminal::shouldClose()) {
            try {
                std::vector<uint8_t> rawCodes = Terminal::read();
                SPDLOG_DEBUG("Read from terminal:");
                HEXDUMP(rawCodes.data(), rawCodes.size());

                // Closing of the terminal happens on the render thread, so it could happen between the while loop checks.
                // Sometimes when the terminal is being closed, a running program will send a message to signal it is being closed (e.g ssh).
                // That message would try to be rendered, but the render thread has finished, calling the font's destructor,
                // which means deleted font data would be read, resulting in a segfault.
                if (Terminal::shouldClose()) {
                    break;
                }

                std::vector<codepoint_t> codepoints =
                    parser.parseAndModifyTermBuf(rawCodes);
                if (codepoints.empty()) {
                    continue;
                }

                for (const codepoint_t c : codepoints) {
                    // Skip control characters
                    if (c >= ' ') {
                        atlasQueue.push(c);
                    }
                }

                Terminal::getBuf(
                    [&font, &renderer](const TerminalBuf& termBuf) {
                        // TODO: Don't recalculate all
                        renderer.makeTextMesh(termBuf.getRows(), font);
                    });

            } catch (TerminalReadException e) {
                if (!Terminal::shouldClose()) {
                    SPDLOG_ERROR("Failed reading from terminal: {}", e.what());
                }
            }
        }
        SPDLOG_DEBUG("Terminal thread finished");
    });

    // Render loop variables
    glm::vec3 charsPos(glm::round(-font.getMetricsInPx().max_advance +
                                  font.getMetricsInPx().max_advance * 0.25),
                       -font.getMetricsInPx().ascender, 0);
    float charsScale = 1.0f;
    glm::vec3 cameraPos(0);
    bool wireframe = false;
    double prevTime = glfwGetTime();
    auto debugData = DebugUI::DebugData{.frameTimeMs = 0,
                                        .charsPos = charsPos,
                                        .charsScale = charsScale,
                                        .cameraPos = cameraPos,
                                        .wireframe = wireframe};
    std::unordered_set<codepoint_t> codepoints;
    size_t prevRows = 0;

    SPDLOG_INFO("Application started");
    while (!glfwWindowShouldClose(m_Window)) {
        renderer.clear();
        renderer.setWireframe(debugData.wireframe);

        // Scroll down when necessary
        Terminal::getBuf([&](const TerminalBuf& termBuf) {
            size_t rows = termBuf.getRows().size();
            if (rows == prevRows) {
                return;
            }

            auto metrics = font.getMetricsInPx();
            // Also subtract initial charsPos.y
            // Scroll down
            while (rows * metrics.height - metrics.ascender >
                   Application::HEIGHT + charsPos.y) {
                charsPos.y += metrics.height;
            }

            // Scroll up
            while (rows * metrics.height < charsPos.y + metrics.height) {
                charsPos.y -= metrics.height;
            }
            prevRows = rows;
        });

        glm::mat4 transform = glm::scale(
            glm::translate(glm::mat4(1.0f), charsPos), glm::vec3(charsScale));
        renderer.setViewMat(glm::translate(glm::mat4(1.0f), cameraPos));

        // When new terminal data appears, update font atlas for new glyphs
        codepoint_t c;
        while (atlasQueue.pop(c)) {
            codepoints.insert(c);
        }
        if (!codepoints.empty()) {
            bool anyNew = font.updateAtlas(codepoints);
            if (anyNew) {
                Terminal::getBuf(
                    [&font, &renderer](const TerminalBuf& termBuf) {
                        renderer.makeTextMesh(termBuf.getRows(), font);
                    });
            }
            codepoints.clear();
        }

        // Debug TerminalBuf
        // Terminal::getBuf([&](const TerminalBuf& termBuf) {
        //     const auto& rows = termBuf.getRows();
        //     for (size_t i = 0; auto& row : rows) {
        //         std::string msg = "row ";
        //         msg += std::to_string(i) + ": ";
        //
        //         for (auto& cell : row) {
        //             if (Parser::isEol(cell.character)) {
        //                 msg += "\\n";
        //             } else {
        //                 msg += (char)cell.character;
        //             }
        //         }
        //         SPDLOG_ERROR(msg.c_str());
        //
        //         i++;
        //     }
        // });

        renderer.drawText(transform, program);

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
    // FIX: Doesn't work on macos, because read() doesn't error for some reason
    Terminal::close();
    assert(m_TerminalThread->joinable());
    m_TerminalThread->join();
    glfwTerminate();
}

GLFWwindow* Application::getWindow() {
    return m_Window;
}
