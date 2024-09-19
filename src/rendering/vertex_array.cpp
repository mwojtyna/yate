#include "vertex_array.hpp"
#include "opengl.hpp"
#include "vertex_buffer.hpp"
#include <cstddef>
#include <spdlog/spdlog.h>

VertexArray::VertexArray() {
    glCall(glGenVertexArrays(1, &m_Id));
    SPDLOG_TRACE("Generated vertex array with id={}", m_Id);
    bind();
}

VertexArray::~VertexArray() {
    glCall(glDeleteVertexArrays(1, &m_Id));
    SPDLOG_TRACE("Deleted vertex array with id={}", m_Id);
}

void VertexArray::addBuffer(const VertexBuffer& vb) const {
    vb.bind();

    glCall(glVertexAttribPointer(0, 3, GL_FLOAT, GL_FALSE, sizeof(Vertex),
                                 (const void*)offsetof(Vertex, pos)));
    glCall(glEnableVertexAttribArray(0));

    glCall(glVertexAttribPointer(1, 4, GL_FLOAT, GL_FALSE, sizeof(Vertex),
                                 (const void*)offsetof(Vertex, color)));
    glCall(glEnableVertexAttribArray(1));

    glCall(glVertexAttribPointer(2, 2, GL_FLOAT, GL_FALSE, sizeof(Vertex),
                                 (const void*)offsetof(Vertex, uv)));
    glCall(glEnableVertexAttribArray(2));

    glCall(glVertexAttribIPointer(3, 1, GL_UNSIGNED_BYTE, sizeof(Vertex),
                                  (const void*)offsetof(Vertex, bg)));
    glCall(glEnableVertexAttribArray(3));

    SPDLOG_TRACE("Added vertex buffer with id={} to vertex array with id={}",
                 vb.getId(), m_Id);
}

void VertexArray::bind() const {
    glCall(glBindVertexArray(m_Id));
}

void VertexArray::unbind() const {
    glCall(glBindVertexArray(0));
}
