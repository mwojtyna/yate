#pragma once

#include "glad/glad.h"

class IndexBuffer {
  public:
    IndexBuffer() = delete;
    IndexBuffer(const GLuint indices[], const GLsizeiptr size);
    ~IndexBuffer();

    void bind() const;
    void unbind() const;

  private:
    GLuint m_Id;
};
