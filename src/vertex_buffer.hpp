#pragma once

#include "error.hpp"
#include "glad/glad.h"
#include "spdlog/spdlog.h"
#include <cstddef>

template <typename T>
class VertexBuffer {
  public:
    VertexBuffer() = delete;
    VertexBuffer(const T *data, const GLsizeiptr size);
    ~VertexBuffer();

    void bind() const;
    void unbind() const;

  private:
    GLuint m_Id;
};

template <typename T>
VertexBuffer<T>::VertexBuffer(const T *data, const GLsizeiptr size) {
    glCall(glGenBuffers(1, &m_Id));
    SPDLOG_TRACE("Generated vertex buffer with id={}", m_Id);
    bind();
    glCall(glBufferData(GL_ARRAY_BUFFER, size, data, GL_STATIC_DRAW));
}

template <typename T>
VertexBuffer<T>::~VertexBuffer() {
    glCall(glDeleteBuffers(1, &m_Id));
    SPDLOG_TRACE("Deleted vertex buffer with id={}", m_Id);
}

template <typename T>
void VertexBuffer<T>::bind() const {
    glCall(glBindBuffer(GL_ARRAY_BUFFER, m_Id));
    SPDLOG_TRACE("Bound vertex buffer with id={}", m_Id);
}

template <typename T>
void VertexBuffer<T>::unbind() const {
    glCall(glBindBuffer(0, m_Id));
    SPDLOG_TRACE("Unbound vertex buffer with id={}", m_Id);
}
