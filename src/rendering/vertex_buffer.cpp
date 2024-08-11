#include "vertex_buffer.hpp"
#include "../error.hpp"
#include "glad/glad.h"
#include "spdlog/spdlog.h"

VertexBuffer::VertexBuffer(const Vertex vertices[], const GLsizei length) {
    glCall(glGenBuffers(1, &m_Id));
    SPDLOG_TRACE("Generated vertex buffer with id={}", m_Id);
    bind();
    glCall(glBufferData(GL_ARRAY_BUFFER, length * sizeof(Vertex), vertices,
                        GL_STATIC_DRAW));
}

VertexBuffer::~VertexBuffer() {
    glCall(glDeleteBuffers(1, &m_Id));
    SPDLOG_TRACE("Deleted vertex buffer with id={}", m_Id);
}

void VertexBuffer::bind() const {
    glCall(glBindBuffer(GL_ARRAY_BUFFER, m_Id));
    SPDLOG_TRACE("Bound vertex buffer with id={}", m_Id);
}

void VertexBuffer::unbind() const {
    glCall(glBindBuffer(GL_ARRAY_BUFFER, 0));
    SPDLOG_TRACE("Unbound vertex buffer with id={}", m_Id);
}

GLuint VertexBuffer::getId() const {
    return m_Id;
}
