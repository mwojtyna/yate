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

void Renderer::drawText(const ParsedChunk& chunk, Font& font,
                        glm::mat4& transform, Program& program) {
    glCall(glClearColor(s_Data->bgColor.r, s_Data->bgColor.g, s_Data->bgColor.b,
                        1.0f));
    glCall(glClear(GL_COLOR_BUFFER_BIT));

    // TODO: Updateable text
    // if (codes == s_Data->codes) {
    //     s_Data->glyphMesh->draw();
    //     s_Data->codes = codes;
    //     return;
    // }
    if (s_Data->glyphMesh == nullptr) {
        s_Data->glyphMesh = std::make_unique<Mesh>(90 * 30 * 4, 90 * 30 * 6,
                                                   transform, program);
    }

    std::vector<Vertex> vertices;
    std::vector<Index> indices;
    glm::vec2 pen(0, -Font::fracToPx(font.getMetrics().ascender));
    for (size_t i = 0, j = 0; i < chunk.text.size(); i++, j += 4) {
        const GlyphPos g = font.getGlyphPos(chunk.text[i], pen);

        vertices.push_back({{pen.x + g.pl, pen.y + g.pb, 0.0f},
                            {1.0f, 1.0f, 1.0f},
                            {g.al, g.ab}}); // left bottom
        vertices.push_back({{pen.x + g.pr, pen.y + g.pb, 0.0f},
                            {1.0f, 1.0f, 1.0f},
                            {g.ar, g.ab}}); // right bottom
        vertices.push_back({{pen.x + g.pr, pen.y + g.pt, 0.0f},
                            {1.0f, 1.0f, 1.0f},
                            {g.ar, g.at}}); // right top
        vertices.push_back({{pen.x + g.pl, pen.y + g.pt, 0.0f},
                            {1.0f, 1.0f, 1.0f},
                            {g.al, g.at}}); // left top

        indices.push_back(j + 0);
        indices.push_back(j + 1);
        indices.push_back(j + 3);

        indices.push_back(j + 1);
        indices.push_back(j + 2);
        indices.push_back(j + 3);
    }

    s_Data->glyphMesh->update(vertices, indices);
    s_Data->glyphMesh->draw();
}

void Renderer::setWireframe(const bool enabled) {
    glCall(glPolygonMode(GL_FRONT_AND_BACK, enabled ? GL_LINE : GL_FILL));
}

void Renderer::setBgColor(const glm::vec3 color) {
    s_Data->bgColor = color;
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
void Renderer::setViewMat(const glm::mat4&& mat) {
    s_Data->viewMat = mat;
}
