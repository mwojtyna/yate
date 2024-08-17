#pragma once

#include "vertex_buffer.hpp"
#include <glad/glad.h>

class VertexArray {
public:
    VertexArray();
    ~VertexArray();

    void addBuffer(const VertexBuffer& vb) const;
    void bind() const;
    void unbind() const;

private:
    GLuint m_Id = 0;
};
