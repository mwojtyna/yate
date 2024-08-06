#include "mesh.hpp"
#include "error.hpp"
#include "glad/glad.h"
#include "index_buffer.hpp"
#include "spdlog/spdlog.h"
#include "vertex_array.hpp"
#include <memory>
#include <string_view>

Mesh::Mesh(const std::string_view name, const GLfloat vertices[],
           const GLsizeiptr verticesSize, const GLuint indices[],
           const GLsizeiptr indicesSize)
    : m_IndicesCount(indicesSize / sizeof(GLuint)), m_Name(name) {
    m_Va = std::make_unique<const VertexArray>();
    m_Ib = std::make_unique<const IndexBuffer>(indices, indicesSize);
    m_Vb = std::make_unique<const VertexBuffer>(vertices, verticesSize);
    m_Va->addBuffer(*m_Vb, VertexArrayLayout::POSITION);
    SPDLOG_DEBUG("Created mesh '{}'", name);
}

Mesh::~Mesh() {
    SPDLOG_DEBUG("Deleted mesh '{}'", m_Name);
}

void Mesh::draw() const {
    m_Va->bind();
    glCall(glDrawElements(GL_TRIANGLES, m_IndicesCount, GL_UNSIGNED_INT,
                          (void *)0));
}
