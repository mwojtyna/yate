#include "renderer.hpp"
#include "../application.hpp"
#include "../error.hpp"
#include "GLFW/glfw3.h"
#include "glad/glad.h"
#include "glm/ext/matrix_clip_space.hpp"
#include "glm/ext/matrix_float4x4.hpp"
#include "glm/fwd.hpp"
#include "mesh.hpp"
#include "program.hpp"
#include "spdlog/spdlog.h"
#include "vertex_buffer.hpp"
#include <cstddef>
#include <string>
#include <vector>

glm::mat4 Renderer::projection = glm::ortho(
    0.0f, (float)Application::WIDTH / (float)Application::HEIGHT, -1.0f, 0.0f);
glm::mat4 Renderer::view = glm::mat4(1.0f);

glm::vec3 Renderer::m_BgColor;
std::string Renderer::m_Data;
Mesh* Renderer::m_CharMesh = nullptr;

void Renderer::initialize() {
    if (!gladLoadGLLoader((GLADloadproc)glfwGetProcAddress)) {
        SPDLOG_ERROR("Failed to initialize OpenGL");
        assert(false);
    }

    glCall(
        SPDLOG_DEBUG("OpenGL version: {}", (GLchar*)glGetString(GL_VERSION)));
    glCall(glEnable(GL_BLEND));
    glCall(glBlendFunc(GL_SRC_ALPHA, GL_ONE_MINUS_SRC_ALPHA));
    glCall(glPixelStorei(GL_UNPACK_ALIGNMENT, 1));

    SPDLOG_DEBUG("Initialized renderer");
}

void Renderer::destroy() {
    delete m_CharMesh;
    SPDLOG_DEBUG("Shutdown renderer");
}

void Renderer::draw(std::string data, glm::mat4& transform, Program& program) {
    glCall(glClearColor(m_BgColor.r, m_BgColor.g, m_BgColor.b, 1.0f));
    glCall(glClear(GL_COLOR_BUFFER_BIT));

    if (data == m_Data && m_CharMesh != nullptr) {
        m_CharMesh->draw();
        return;
    }

    // TODO: Don't allocate new buffers every time data is changed
    std::vector<Vertex> vertices;
    std::vector<GLuint> indices;
    for (size_t i = 0, j = 0; i < data.length(); i++, j += 4) {
        vertices.push_back({{0.0f + i, 0.0f, 0.0f},
                            {0.0f, 0.0f, 0.0f, 1.0f},
                            {0.0f, 0.0f}}); // top left
        vertices.push_back({{1.0f + i, 0.0f, 0.0f},
                            {1.0f, 0.0f, 0.0f, 1.0f},
                            {1.0f, 0.0f}}); // top right
        vertices.push_back({{1.0f + i, -1.0f, 0.0f},
                            {0.0f, 1.0f, 0.0f, 1.0f},
                            {1.0f, 1.0f}}); // bottom right
        vertices.push_back({{0.0f + i, -1.0f, 0.0f},
                            {0.0f, 0.0f, 1.0f, 1.0f},
                            {0.0f, 1.0f}}); // bottom left

        indices.push_back(j + 0);
        indices.push_back(j + 1);
        indices.push_back(j + 3);

        indices.push_back(j + 1);
        indices.push_back(j + 2);
        indices.push_back(j + 3);
    }

    m_Data = data;
    m_CharMesh = new Mesh(vertices.data(), vertices.size(), indices.data(),
                          indices.size(), transform, program);
    m_CharMesh->draw();
}

void Renderer::setWireframe(const bool enabled) {
    glCall(glPolygonMode(GL_FRONT_AND_BACK, enabled ? GL_LINE : GL_FILL));
}

void Renderer::setBgColor(const glm::vec3 color) {
    m_BgColor = color;
}
