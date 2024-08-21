#include "vertex_buffer.hpp"
#include "opengl.hpp"
#include <spdlog/spdlog.h>

VertexBuffer::VertexBuffer(const GLsizei count) {
    glCall(glGenBuffers(1, &m_Id));
    SPDLOG_TRACE("Generated vertex buffer with id={}", m_Id);
    bind();
    glCall(glBufferData(GL_ARRAY_BUFFER, count * sizeof(Vertex), nullptr,
                        GL_DYNAMIC_DRAW));
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

void VertexBuffer::update(std::vector<Vertex>& vertices) {
    bind();
    glCall(glBufferSubData(GL_ARRAY_BUFFER, 0, vertices.size() * sizeof(Vertex),
                           vertices.data()));
}

GLuint VertexBuffer::getId() const {
    return m_Id;
}
