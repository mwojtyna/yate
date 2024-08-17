#include "font.hpp"
#include "glad/glad.h"
// GLFW (include after glad)
#include "GLFW/glfw3.h"

#include "../error.hpp"
#include "glm/ext/matrix_float4x4.hpp"
#include "glm/fwd.hpp"
#include "index_buffer.hpp"
#include "program.hpp"
#include "renderer.hpp"
#include "spdlog/spdlog.h"
#include "vertex_buffer.hpp"
#include <cstddef>
#include <memory>
#include <string>
#include <vector>

RendererData* Renderer::s_Data;

void Renderer::initialize() {
    if (!gladLoadGLLoader((GLADloadproc)glfwGetProcAddress)) {
        SPDLOG_ERROR("Failed to initialize OpenGL");
        std::exit(1);
    }

    glCall(
        SPDLOG_DEBUG("OpenGL version: {}", (GLchar*)glGetString(GL_VERSION)));
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

void Renderer::drawText(std::string codes, Font& font, glm::mat4& transform,
                        Program& program) {
    glCall(glClearColor(s_Data->bgColor.r, s_Data->bgColor.g, s_Data->bgColor.b,
                        1.0f));
    glCall(glClear(GL_COLOR_BUFFER_BIT));

    if (codes == s_Data->codes) {
        s_Data->glyphMesh->draw();
        return;
    }

    if (s_Data->glyphMesh == nullptr) {
        s_Data->glyphMesh = std::make_unique<Mesh>(90 * 30 * 4, 90 * 30 * 6,
                                                   transform, program);
    }

    std::vector<Vertex> vertices;
    std::vector<Index> indices;
    glm::vec2 prevPos(0, 0);
    for (size_t i = 0, j = 0; i < codes.length(); i++, j += 4) {
        Glyph g = font.getGlyph(codes[i]);

        vertices.push_back({{prevPos.x, -g.h, 0.0f},
                            {1.0f, 1.0f, 1.0f, 1.0f},
                            {g.l, g.b}}); // bottom left
        vertices.push_back({{prevPos.x + g.w, -g.h, 0.0f},
                            {1.0f, 1.0f, 1.0f, 1.0f},
                            {g.r, g.b}}); // bottom right
        vertices.push_back({{prevPos.x + g.w, 0.0f, 0.0f},
                            {1.0f, 1.0f, 1.0f, 1.0f},
                            {g.r, g.t}}); // top right
        vertices.push_back({{prevPos.x, 0.0f, 0.0f},
                            {1.0f, 1.0f, 1.0f, 1.0f},
                            {g.l, g.t}}); // top left

        prevPos.x += g.w + g.advance;

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

void Renderer::setViewMat(glm::mat4& mat) {
    s_Data->viewMat = mat;
}
void Renderer::setViewMat(glm::mat4&& mat) {
    s_Data->viewMat = mat;
}
