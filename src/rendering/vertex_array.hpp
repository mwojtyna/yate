#pragma once

#include "opengl.hpp"
#include "vertex_buffer.hpp"

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
