#include "vertex_array.hpp"
#include "error.hpp"
#include "spdlog/spdlog.h"
#include "vertex_buffer.hpp"

VertexArray::VertexArray() {
    glCall(glGenVertexArrays(1, &m_Id));
    SPDLOG_TRACE("Generated vertex array with id={}", m_Id);
    bind();
}

VertexArray::~VertexArray() {
    glCall(glDeleteVertexArrays(1, &m_Id));
    SPDLOG_TRACE("Deleted vertex array with id={}", m_Id);
}

void VertexArray::addBuffer(const VertexBuffer &vb,
                            const VertexArrayLayout layout) const {
    vb.bind();

    switch (layout) {
    case VertexArrayLayout::POSITION: {
        glCall(glVertexAttribPointer(0, 3, GL_FLOAT, GL_FALSE,
                                     3 * sizeof(GLfloat), (void *)0));
        glCall(glEnableVertexAttribArray(0));
        break;
    }
    }

    SPDLOG_TRACE("Added vertex buffer with id={} to vertex array with id={}",
                 vb.getId(), m_Id);
}

void VertexArray::bind() const {
    glCall(glBindVertexArray(m_Id));
    SPDLOG_TRACE("Bound vertex array with id={}", m_Id);
}

void VertexArray::unbind() const {
    glCall(glBindVertexArray(0));
    SPDLOG_TRACE("Unbound vertex array with id={}", m_Id);
}
