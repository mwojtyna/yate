#include "mesh.hpp"
#include "../error.hpp"
#include "glad/glad.h"
#include "glm/ext/matrix_float4x4.hpp"
#include "index_buffer.hpp"
#include "renderer.hpp"
#include "spdlog/spdlog.h"
#include "vertex_array.hpp"
#include "vertex_buffer.hpp"
#include <memory>

Mesh::Mesh(const Vertex vertices[], const GLsizei verticesCount,
           const GLuint indices[], const GLsizei indicesCount,
           glm::mat4& transform, Program& program)
    : m_IndicesCount(indicesCount), m_Transform(transform), m_Program(program) {
    m_Va = std::make_unique<const VertexArray>();
    m_Ib = std::make_unique<const IndexBuffer>(indices, indicesCount);
    m_Vb = std::make_unique<const VertexBuffer>(vertices, verticesCount);
    m_Va->addBuffer(*m_Vb);

    SPDLOG_DEBUG("Created mesh");
}

Mesh::~Mesh() {
    SPDLOG_DEBUG("Deleted mesh");
}

void Mesh::draw() const {
    glm::mat4 view = glm::mat4(1.0f);
    glm::mat4 mvp = Renderer::PROJECTION * view * m_Transform;
    m_Program.setUniformMatrix4f("u_MVP", mvp);

    m_Va->bind();
    glCall(glDrawElements(GL_TRIANGLES, m_IndicesCount, GL_UNSIGNED_INT,
                          (const void*)0));
}
