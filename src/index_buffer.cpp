#include "./index_buffer.hpp"
#include "error.hpp"
#include "glad/glad.h"
#include "spdlog/spdlog.h"
#include <cstddef>

IndexBuffer::IndexBuffer(const GLuint indices[], const GLsizeiptr size) {
    glCall(glGenBuffers(1, &m_Id));
    SPDLOG_TRACE("Generated index buffer with id={}", m_Id);
    bind();
    glCall(
        glBufferData(GL_ELEMENT_ARRAY_BUFFER, size, indices, GL_STATIC_DRAW));
}

IndexBuffer::~IndexBuffer() {
    glCall(glDeleteBuffers(1, &m_Id));
    SPDLOG_TRACE("Deleted vertex buffer with id={}", m_Id);
}

void IndexBuffer::bind() const {
    glCall(glBindBuffer(GL_ELEMENT_ARRAY_BUFFER, m_Id));
    SPDLOG_TRACE("Bound vertex buffer with id={}", m_Id);
}

void IndexBuffer::unbind() const {
    glCall(glBindBuffer(GL_ELEMENT_ARRAY_BUFFER, 0));
    SPDLOG_TRACE("Unbound vertex buffer with id={}", m_Id);
}
