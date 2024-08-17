#pragma once

#include <glad/glad.h>
#include <vector>

using Index = GLuint;

class IndexBuffer {
public:
    IndexBuffer() = delete;
    IndexBuffer(GLsizei count);
    ~IndexBuffer();

    void bind() const;
    void unbind() const;
    void update(std::vector<Index>& indices);

    GLsizei getCount() const;

private:
    GLuint m_Id = 0;
    GLsizei m_Count = 0;
};
