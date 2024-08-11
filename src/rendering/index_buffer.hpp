#pragma once

#include "glad/glad.h"

class IndexBuffer {
public:
    IndexBuffer() = delete;
    IndexBuffer(const GLuint indices[], const GLsizei length);
    ~IndexBuffer();

    void bind() const;
    void unbind() const;

private:
    GLuint m_Id = 0;
};
