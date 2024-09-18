#include "renderer.hpp"
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

RendererData* Renderer::s_Data;

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

void Renderer::drawText(const std::vector<Cell>& cells, Font& font,
                        const glm::mat4& transform, Program& program) {
    glCall(glClear(GL_COLOR_BUFFER_BIT));

    if (s_Data->glyphMesh == nullptr) {
        // TODO: Proper cell sizing
        s_Data->glyphMesh = std::make_unique<Mesh>(
            Application::WIDTH * Application::HEIGHT * 4,
            Application::WIDTH * Application::HEIGHT * 6, transform, program);
    }

    std::vector<Vertex> vertices;
    std::vector<index_t> indices;
    glm::vec2 pen(0);
    size_t curIndex = 0;
    for (const Cell& cell : cells) {
        const GlyphPos g = font.getGlyphPos(cell, pen);

        vertices.push_back({{pen.x + g.pl, pen.y + g.pb, 0.0f},
                            cell.fgColor,
                            {g.al, g.ab}}); // left bottom
        vertices.push_back({{pen.x + g.pr, pen.y + g.pb, 0.0f},
                            cell.fgColor,
                            {g.ar, g.ab}}); // right bottom
        vertices.push_back({{pen.x + g.pr, pen.y + g.pt, 0.0f},
                            cell.fgColor,
                            {g.ar, g.at}}); // right top
        vertices.push_back({{pen.x + g.pl, pen.y + g.pt, 0.0f},
                            cell.fgColor,
                            {g.al, g.at}}); // left top

        indices.push_back(curIndex + 0);
        indices.push_back(curIndex + 1);
        indices.push_back(curIndex + 3);

        indices.push_back(curIndex + 1);
        indices.push_back(curIndex + 2);
        indices.push_back(curIndex + 3);
        curIndex += 4;
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

glm::mat4& Renderer::getProjectionMat() {
    return s_Data->projectionMat;
}

glm::mat4& Renderer::getViewMat() {
    return s_Data->viewMat;
}

void Renderer::setViewMat(const glm::mat4& mat) {
    s_Data->viewMat = mat;
}
