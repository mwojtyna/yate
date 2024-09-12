#pragma once

#include "opengl.hpp"
#include <vector>

using index_t = GLuint;

class IndexBuffer {
public:
    IndexBuffer() = delete;
    IndexBuffer(GLsizei count);
    ~IndexBuffer();

    void bind() const;
    void unbind() const;
    void update(std::vector<index_t>& indices);

    GLsizei getCount() const;

private:
    GLuint m_Id = 0;
    GLsizei m_Count = 0;
};
