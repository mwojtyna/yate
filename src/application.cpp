#include "application.hpp"
#include "debug_ui.hpp"
#include "rendering/font.hpp"
#include "rendering/program.hpp"
#include "rendering/renderer.hpp"
#include "shaders/text.frag.hpp"
#include "shaders/text.vert.hpp"
#include "terminal/event_handler.hpp"
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

void Application::start() {
    spdlog::cfg::load_env_levels();

    SDL_version sdlVersion;
    SDL_GetVersion(&sdlVersion);
    SPDLOG_INFO("SDL version: {}.{}.{}", sdlVersion.major, sdlVersion.major,
                sdlVersion.patch);

    // Prefer wayland over xwayland
    SDL_SetHintWithPriority(SDL_HINT_VIDEODRIVER, "wayland,x11,cocoa",
                            SDL_HINT_NORMAL);

    if (SDL_Init(SDL_INIT_VIDEO | SDL_INIT_TIMER) < 0) {
        FATAL("Failed to initialize SDL: {}", SDL_GetError());
    }
    SDL_GL_SetAttribute(SDL_GL_CONTEXT_MAJOR_VERSION, 3);
    SDL_GL_SetAttribute(SDL_GL_CONTEXT_MINOR_VERSION, 3);
    SDL_GL_SetAttribute(SDL_GL_CONTEXT_PROFILE_MASK,
                        SDL_GL_CONTEXT_PROFILE_CORE);

    m_Window = SDL_CreateWindow("yate", SDL_WINDOWPOS_UNDEFINED,
                                SDL_WINDOWPOS_UNDEFINED, Application::WIDTH,
                                Application::HEIGHT,
                                SDL_WINDOW_OPENGL | SDL_WINDOW_ALLOW_HIGHDPI);
    if (m_Window == nullptr) {
        FATAL("Failed to create window: {}", SDL_GetError());
    }
    int realWidth;
    SDL_GL_GetDrawableSize(m_Window, &realWidth, nullptr);
    float contentScale = (float)realWidth / Application::WIDTH;

    Renderer renderer(m_Window, contentScale);
    renderer.setBgColor(glm::vec3(0.10f, 0.11f, 0.15f));
    Program program(textVertexShader, textFragmentShader);
    Font font("/usr/share/fonts/TTF/JetBrainsMonoNerdFont-Regular.ttf",
              16 * contentScale);
    DebugUI debugUI(m_Window, renderer.getContext());
    EventHandler eventHandler(m_Window);

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
                // Sometimes when the terminal is being closed, a running program will send a message to signal it is being closed (e.g. ssh).
                // That message would try to be rendered, but the render thread has finished, calling the font's destructor,
                // which means deleted font data would be read, resulting in a segfault.
                if (Terminal::shouldClose()) {
                    break;
                }

                std::unordered_set<codepoint_t> codepoints =
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

            } catch (TerminalReadException& e) {
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
    uint64_t prevTime = SDL_GetTicks();
    auto debugData = DebugUI::DebugData{.frameTimeMs = 0,
                                        .charsPos = charsPos,
                                        .charsScale = charsScale,
                                        .cameraPos = cameraPos,
                                        .wireframe = wireframe};
    std::unordered_set<codepoint_t> codepoints;
    size_t prevRows = 0;
    bool quit = false;

    SPDLOG_INFO("Application started");
    while (!quit) {
        eventHandler.handleEvents(quit, debugUI);
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
                   Application::HEIGHT * contentScale + charsPos.y) {
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

        debugData.frameTimeMs = SDL_GetTicks() - prevTime;
        prevTime = SDL_GetTicks();
        debugUI.draw(debugData);

        SDL_GL_SwapWindow(m_Window);
    }
}

Application::~Application() {
    SPDLOG_INFO("Application exiting");
    // FIX: Doesn't work on macos, because read() doesn't error for some reason
    Terminal::close();
    assert(m_TerminalThread->joinable());
    m_TerminalThread->join();
    SDL_Quit();
}

SDL_Window* Application::getWindow() const {
    return m_Window;
}
