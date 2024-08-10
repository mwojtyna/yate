#include "mesh.hpp"
#include "error.hpp"
#include "glad/glad.h"
#include "index_buffer.hpp"
#include "spdlog/spdlog.h"
#include "vertex_array.hpp"
#include "vertex_buffer.hpp"
#include <memory>

Mesh::Mesh(const Vertex vertices[], const GLsizei verticesCount,
           const GLuint indices[], const GLsizei indicesCount)
    : m_IndicesCount(indicesCount) {
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
    m_Va->bind();
    glCall(glDrawElements(GL_TRIANGLES, m_IndicesCount, GL_UNSIGNED_INT,
                          (const void*)0));
}
