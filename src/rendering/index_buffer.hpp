#pragma once

#include "glad/glad.h"
#include <vector>

class IndexBuffer {
public:
    IndexBuffer() = delete;
    IndexBuffer(const std::vector<GLuint>& indices);
    ~IndexBuffer();

    void bind() const;
    void unbind() const;

private:
    GLuint m_Id = 0;
};
