#include "renderer.hpp"
#include "../terminal/codes.hpp"
#include "../terminal/terminal.hpp"
#include "../terminal/types.hpp"
#include "../utils.hpp"
#include "font.hpp"
#include "index_buffer.hpp"
#include "opengl.hpp"
#include "program.hpp"
#include "vertex_buffer.hpp"
#include <cstddef>
#include <glm/ext/matrix_float4x4.hpp>
#include <glm/fwd.hpp>
#include <memory>
#include <spdlog/spdlog.h>
#include <vector>

Renderer::RendererData* Renderer::s_Data;

void Renderer::initialize() {
    if (!gladLoadGLLoader((GLADloadproc)glfwGetProcAddress)) {
        FATAL("Failed to initialize OpenGL");
    }

    glCall(SPDLOG_INFO("OpenGL version: {}", (GLchar*)glGetString(GL_VERSION)));
    glCall(glEnable(GL_BLEND));
    glCall(glBlendFunc(GL_SRC_ALPHA, GL_ONE_MINUS_SRC_ALPHA));
    glCall(glPixelStorei(GL_UNPACK_ALIGNMENT, 1));
    glCall(glDepthMask(GL_FALSE));

    s_Data = new RendererData();
    SPDLOG_DEBUG("Initialized renderer");
}

void Renderer::destroy() {
    delete s_Data;
    SPDLOG_DEBUG("Shutdown renderer");
}

void Renderer::drawText(const std::vector<std::vector<Cell>>& cells, Font& font,
                        const glm::mat4& transform, Program& program) {

    if (s_Data->glyphMesh == nullptr) {
        // TODO: Proper cell sizing
        s_Data->glyphMesh = std::make_unique<Mesh>(
            Application::WIDTH * Application::HEIGHT * 4,
            Application::WIDTH * Application::HEIGHT * 6, transform, program);
    }

    std::vector<Vertex> vertices;
    std::vector<index_t> indices;
    size_t curIndex = 0;
    glm::vec2 pen(0);
    cursor_t cursor = Terminal::getCursor();

    const double bgOffsetY = font.getMetrics().descender;
    const glm::vec2 bgSize(font.getMetrics().max_advance,
                           font.getMetrics().height);

    for (size_t y = 0; y < cells.size(); y++) {
        const auto& row = cells[y];

        for (size_t x = 0; x < row.size(); x++) {
            const auto& cell = row[x];
            const GlyphPos g = font.getGlyphPos(cell, pen);
            const bool isCursor = y == cursor.y && x == cursor.x;

            if (!cell.lineEnd && cell.character != c0::HT || isCursor) {
                const glm::vec4 bgColor =
                    isCursor ? glm::vec4(1) : cell.bgColor;

                // Background
                vertices.push_back(
                    {.pos = {pen.x, pen.y + bgSize.y + bgOffsetY, 0.0f},
                     .color = bgColor,
                     .bg = true}); // left bottom
                vertices.push_back({.pos = {pen.x + bgSize.x,
                                            pen.y + bgSize.y + bgOffsetY, 0.0f},
                                    .color = bgColor,
                                    .bg = true}); // right bottom
                vertices.push_back(
                    {.pos = {pen.x + bgSize.x, pen.y + bgOffsetY, 0.0f},
                     .color = bgColor,
                     .bg = true}); // right top
                vertices.push_back({.pos = {pen.x, pen.y + bgOffsetY, 0.0f},
                                    .color = bgColor,
                                    .bg = true}); // left top

                // Background first triangle
                indices.push_back(curIndex + 0);
                indices.push_back(curIndex + 1);
                indices.push_back(curIndex + 3);

                // Background second triangle
                indices.push_back(curIndex + 1);
                indices.push_back(curIndex + 2);
                indices.push_back(curIndex + 3);
                curIndex += 4;
            }

            // Foreground
            const glm::vec4 fgColor =
                isCursor ? glm::vec4(0, 0, 0, 1) : cell.fgColor;

            vertices.push_back({.pos = {pen.x + g.pl, pen.y + g.pb, 0.0f},
                                .color = fgColor,
                                .uv = {g.al, g.ab},
                                .bg = false}); // left bottom
            vertices.push_back({.pos = {pen.x + g.pr, pen.y + g.pb, 0.0f},
                                .color = fgColor,
                                .uv = {g.ar, g.ab},
                                .bg = false}); // right bottom
            vertices.push_back({.pos = {pen.x + g.pr, pen.y + g.pt, 0.0f},
                                .color = fgColor,
                                .uv = {g.ar, g.at},
                                .bg = false}); // right top
            vertices.push_back({.pos = {pen.x + g.pl, pen.y + g.pt, 0.0f},
                                .color = fgColor,
                                .uv = {g.al, g.at},
                                .bg = false}); // left top

            // Foreground first triangle
            indices.push_back(curIndex + 0);
            indices.push_back(curIndex + 1);
            indices.push_back(curIndex + 3);

            // Foreground second triangle
            indices.push_back(curIndex + 1);
            indices.push_back(curIndex + 2);
            indices.push_back(curIndex + 3);
            curIndex += 4;
        }

        // Draw additional quad when cursor is at the end of the row
        if (cursor.y == y && cursor.x == row.size()) {
            const Cell cursorCell = {.bgColor = glm::vec4(1)};
            font.getGlyphPos(cursorCell, pen);

            // Background
            vertices.push_back(
                {.pos = {pen.x, pen.y + bgSize.y + bgOffsetY, 0.0f},
                 .color = cursorCell.bgColor,
                 .bg = true}); // left bottom
            vertices.push_back(
                {.pos = {pen.x + bgSize.x, pen.y + bgSize.y + bgOffsetY, 0.0f},
                 .color = cursorCell.bgColor,
                 .bg = true}); // right bottom
            vertices.push_back(
                {.pos = {pen.x + bgSize.x, pen.y + bgOffsetY, 0.0f},
                 .color = cursorCell.bgColor,
                 .bg = true}); // right top
            vertices.push_back({.pos = {pen.x, pen.y + bgOffsetY, 0.0f},
                                .color = cursorCell.bgColor,
                                .bg = true}); // left top

            // Background first triangle
            indices.push_back(curIndex + 0);
            indices.push_back(curIndex + 1);
            indices.push_back(curIndex + 3);

            // Background second triangle
            indices.push_back(curIndex + 1);
            indices.push_back(curIndex + 2);
            indices.push_back(curIndex + 3);
            curIndex += 4;
        }
    }

    s_Data->glyphMesh->update(vertices, indices);
    s_Data->glyphMesh->draw();
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

glm::mat4& Renderer::getProjectionMat() {
    return s_Data->projectionMat;
}

glm::mat4& Renderer::getViewMat() {
    return s_Data->viewMat;
}

void Renderer::setViewMat(const glm::mat4& mat) {
    s_Data->viewMat = mat;
}
