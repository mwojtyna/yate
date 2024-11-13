#include "renderer.hpp"
#include "../application.hpp"
#include "../terminal/parser.hpp"
#include "../terminal/terminal.hpp"
#include "../terminal/types.hpp"
#include "../utils.hpp"
#include "opengl.hpp"
#include <cstddef>
#include <spdlog/spdlog.h>
#include <vector>

Renderer::Renderer(SDL_Window* window, float contentScale = 1)
    : m_ProjectionMat(
          glm::ortho(0.0f, ((float)Application::WIDTH * contentScale),
                     (-(float)Application::HEIGHT * contentScale), 0.0f)),
      m_ViewMat(1), m_ContentScale(contentScale) {
    SDL_GLContext ctx = SDL_GL_CreateContext(window);
    SDL_GL_SetSwapInterval(1);

    if (!gladLoadGLLoader((GLADloadproc)SDL_GL_GetProcAddress)) {
        FATAL("Failed to initialize OpenGL");
    }

    glCall(SPDLOG_INFO("OpenGL version: {}", (GLchar*)glGetString(GL_VERSION)));

    glCall(glEnable(GL_BLEND));
    glCall(glBlendFunc(GL_SRC_ALPHA, GL_ONE_MINUS_SRC_ALPHA));
    glCall(glPixelStorei(GL_UNPACK_ALIGNMENT, 1));

    SPDLOG_DEBUG("Initialized renderer");
}

void Renderer::makeTextMesh(const std::vector<std::vector<Cell>>& cells,
                            Font& font) {
    m_Vertices.clear();
    m_Indices.clear();

    std::vector<index_t> bgIndices, fgIndices;
    size_t curIndex = 0;
    glm::vec2 pen(0);
    cursor_t cursor = Terminal::getCursor();

    const double bgOffsetY = font.getMetricsInPx().descender;
    const glm::vec2 bgSize(font.getMetricsInPx().max_advance,
                           font.getMetricsInPx().height);

    for (size_t y = 0; y < cells.size(); y++) {
        const auto& row = cells[y];

        for (size_t x = 0; x < row.size(); x++) {
            const auto& cell = row[x];
            const GlyphPos g = font.getGlyphPos(cell, pen);
            const bool isCursor = y == cursor.y && x == cursor.x;

#ifndef NDEBUG
            assert(!Parser::isEol(cell.character));
#endif

            if (cell.character != c0::HT || isCursor) {
                const glm::vec4 bgColor =
                    isCursor ? glm::vec4(1) : cell.bgColor;

                // Background
                m_Vertices.push_back(
                    {.pos = {pen.x, pen.y + bgSize.y + bgOffsetY, 0.0f},
                     .color = bgColor,
                     .bg = true}); // left bottom
                m_Vertices.push_back(
                    {.pos = {pen.x + bgSize.x, pen.y + bgSize.y + bgOffsetY,
                             0.0f},
                     .color = bgColor,
                     .bg = true}); // right bottom
                m_Vertices.push_back(
                    {.pos = {pen.x + bgSize.x, pen.y + bgOffsetY, 0.0f},
                     .color = bgColor,
                     .bg = true}); // right top
                m_Vertices.push_back({.pos = {pen.x, pen.y + bgOffsetY, 0.0f},
                                      .color = bgColor,
                                      .bg = true}); // left top

                // Background first triangle
                bgIndices.push_back(curIndex + 0);
                bgIndices.push_back(curIndex + 1);
                bgIndices.push_back(curIndex + 3);

                // Background second triangle
                bgIndices.push_back(curIndex + 1);
                bgIndices.push_back(curIndex + 2);
                bgIndices.push_back(curIndex + 3);
                curIndex += 4;
            }

            // Foreground
            const glm::vec4 fgColor =
                isCursor ? glm::vec4(0, 0, 0, 1) : cell.fgColor;

            m_Vertices.push_back({.pos = {pen.x + g.pl, pen.y + g.pb, 0.0f},
                                  .color = fgColor,
                                  .uv = {g.al, g.ab},
                                  .bg = false}); // left bottom
            m_Vertices.push_back({.pos = {pen.x + g.pr, pen.y + g.pb, 0.0f},
                                  .color = fgColor,
                                  .uv = {g.ar, g.ab},
                                  .bg = false}); // right bottom
            m_Vertices.push_back({.pos = {pen.x + g.pr, pen.y + g.pt, 0.0f},
                                  .color = fgColor,
                                  .uv = {g.ar, g.at},
                                  .bg = false}); // right top
            m_Vertices.push_back({.pos = {pen.x + g.pl, pen.y + g.pt, 0.0f},
                                  .color = fgColor,
                                  .uv = {g.al, g.at},
                                  .bg = false}); // left top

            // Foreground first triangle
            fgIndices.push_back(curIndex + 0);
            fgIndices.push_back(curIndex + 1);
            fgIndices.push_back(curIndex + 3);

            // Foreground second triangle
            fgIndices.push_back(curIndex + 1);
            fgIndices.push_back(curIndex + 2);
            fgIndices.push_back(curIndex + 3);

            curIndex += 4;
        }

        // Draw additional quad when cursor is at the end of the row
        if (cursor.y == y && cursor.x == row.size()) {
            const Cell cursorCell = {.bgColor = glm::vec4(1)};
            font.getGlyphPos(cursorCell, pen);

            // Background
            m_Vertices.push_back(
                {.pos = {pen.x, pen.y + bgSize.y + bgOffsetY, 0.0f},
                 .color = cursorCell.bgColor,
                 .bg = true}); // left bottom
            m_Vertices.push_back(
                {.pos = {pen.x + bgSize.x, pen.y + bgSize.y + bgOffsetY, 0.0f},
                 .color = cursorCell.bgColor,
                 .bg = true}); // right bottom
            m_Vertices.push_back(
                {.pos = {pen.x + bgSize.x, pen.y + bgOffsetY, 0.0f},
                 .color = cursorCell.bgColor,
                 .bg = true}); // right top
            m_Vertices.push_back({.pos = {pen.x, pen.y + bgOffsetY, 0.0f},
                                  .color = cursorCell.bgColor,
                                  .bg = true}); // left top

            // Background first triangle
            bgIndices.push_back(curIndex + 0);
            bgIndices.push_back(curIndex + 1);
            bgIndices.push_back(curIndex + 3);

            // Background second triangle
            bgIndices.push_back(curIndex + 1);
            bgIndices.push_back(curIndex + 2);
            bgIndices.push_back(curIndex + 3);
            curIndex += 4;
        }

        pen.x = 0;
        pen.y -= font.getMetricsInPx().height;
    }

    m_Indices.insert(m_Indices.end(), bgIndices.begin(), bgIndices.end());
    m_Indices.insert(m_Indices.end(), fgIndices.begin(), fgIndices.end());
}

void Renderer::drawText(const glm::mat4& transform, Program& program) {
    if (m_GlyphMesh == nullptr) {
        // TODO: Proper cell sizing
        m_GlyphMesh = std::make_unique<Mesh>(
            Application::WIDTH * Application::HEIGHT * 4,
            Application::WIDTH * Application::HEIGHT * 6, transform, program);
    }

    m_GlyphMesh->update(m_Vertices, m_Indices);
    m_GlyphMesh->draw(m_ProjectionMat, m_ViewMat);
}

void Renderer::setWireframe(const bool enabled) {
    glCall(glPolygonMode(GL_FRONT_AND_BACK, enabled ? GL_LINE : GL_FILL));
}

void Renderer::setBgColor(const glm::vec3& color) {
    glCall(glClearColor(color.r, color.g, color.b, 1.0f));
}

void Renderer::clear() {
    glCall(glClear(GL_COLOR_BUFFER_BIT));
}

SDL_GLContext Renderer::getContext() const {
    return m_GlContext;
}

void Renderer::setViewMat(const glm::mat4& mat) {
    m_ViewMat = mat;
}
