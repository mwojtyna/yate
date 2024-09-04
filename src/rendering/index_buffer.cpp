#include "index_buffer.hpp"
#include "opengl.hpp"
#include <spdlog/spdlog.h>

IndexBuffer::IndexBuffer(GLsizei count) : m_Count(count) {
    glCall(glGenBuffers(1, &m_Id));
    bind();
    glCall(glBufferData(GL_ELEMENT_ARRAY_BUFFER, count * sizeof(Index), nullptr,
                        GL_DYNAMIC_DRAW));
}

IndexBuffer::~IndexBuffer() {
    glCall(glDeleteBuffers(1, &m_Id));
}

void IndexBuffer::bind() const {
    glCall(glBindBuffer(GL_ELEMENT_ARRAY_BUFFER, m_Id));
}

void IndexBuffer::unbind() const {
    glCall(glBindBuffer(GL_ELEMENT_ARRAY_BUFFER, 0));
}

void IndexBuffer::update(std::vector<Index>& indices) {
    bind();
    glCall(glBufferSubData(GL_ELEMENT_ARRAY_BUFFER, 0,
                           indices.size() * sizeof(Index), indices.data()));
}

GLsizei IndexBuffer::getCount() const {
    return m_Count;
}
