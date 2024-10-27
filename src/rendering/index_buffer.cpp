#include "index_buffer.hpp"
#include "opengl.hpp"
#include <spdlog/spdlog.h>

IndexBuffer::IndexBuffer(const GLsizei maxCount) {
    glCall(glGenBuffers(1, &m_Id));
    bind();
    glCall(glBufferData(GL_ELEMENT_ARRAY_BUFFER, maxCount * sizeof(index_t),
                        nullptr, GL_DYNAMIC_DRAW));
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

void IndexBuffer::update(std::vector<index_t>& indices) {
    bind();
    m_Count = indices.size();
    glCall(glBufferSubData(GL_ELEMENT_ARRAY_BUFFER, 0,
                           indices.size() * sizeof(index_t), indices.data()));
}

GLsizei IndexBuffer::getCount() const {
    return m_Count;
}
