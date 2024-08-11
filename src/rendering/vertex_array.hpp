#pragma once

#include "glad/glad.h"
#include "vertex_buffer.hpp"

class VertexArray {
public:
    VertexArray();
    ~VertexArray();

    void addBuffer(const VertexBuffer& vb) const;
    void bind() const;
    void unbind() const;

private:
    constexpr static GLsizei STRIDE = sizeof(Vertex);
    GLuint m_Id = 0;
};
