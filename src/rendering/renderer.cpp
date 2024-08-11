#include "renderer.hpp"
#include "../application.hpp"
#include "../error.hpp"
#include "glad/glad.h"
#include "glm/ext/matrix_clip_space.hpp"
#include "glm/ext/matrix_float4x4.hpp"
#include "glm/fwd.hpp"
#include "mesh.hpp"
#include "program.hpp"
#include "vertex_buffer.hpp"
#include <cstddef>
#include <string>
#include <vector>

glm::mat4 Renderer::PROJECTION =
    glm::ortho(0.0f, Application::ASPECT* SCALE, -SCALE, 0.0f);

void Renderer::draw(std::string data, glm::mat4& transform, Program& program) {
    glCall(glClearColor(m_BgColor.r, m_BgColor.g, m_BgColor.b, 1.0f));
    glCall(glClear(GL_COLOR_BUFFER_BIT));

    if (data == m_Data) {
        m_Mesh->draw();
        return;
    }

    // TODO: Don't allocate new buffers every time data is changed
    std::vector<Vertex> vertices;
    std::vector<GLuint> indices;
    for (size_t i = 0, j = 0; i < data.length(); i++, j += 4) {
        vertices.push_back({{0.0f + i, 0.0f, 0.0f},
                            {0.0f, 0.0f, 0.0f, 1.0f},
                            {0.0f, 1.0f}}); // top left
        vertices.push_back({{1.0f + i, 0.0f, 0.0f},
                            {1.0f, 0.0f, 0.0f, 1.0f},
                            {1.0f, 1.0f}}); // top right
        vertices.push_back({{1.0f + i, -1.0f, 0.0f},
                            {0.0f, 1.0f, 0.0f, 1.0f},
                            {1.0f, 0.0f}}); // bottom right
        vertices.push_back({{0.0f + i, -1.0f, 0.0f},
                            {0.0f, 0.0f, 1.0f, 1.0f},
                            {0.0f, 0.0f}}); // bottom left

        indices.push_back(j + 0);
        indices.push_back(j + 1);
        indices.push_back(j + 3);

        indices.push_back(j + 1);
        indices.push_back(j + 2);
        indices.push_back(j + 3);
    }

    m_Data = data;
    m_Mesh =
        std::make_unique<Mesh>(vertices.data(), vertices.size(), indices.data(),
                               indices.size(), transform, program);
    m_Mesh->draw();
}

void Renderer::setWireframe(const bool enabled) const {
    glCall(glPolygonMode(GL_FRONT_AND_BACK, enabled ? GL_LINE : GL_FILL));
}

void Renderer::setBgColor(const glm::vec3 color) {
    m_BgColor = color;
}
