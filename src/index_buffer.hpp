#pragma once

#include "glad/glad.h"
#include <cstddef>

class IndexBuffer {
  public:
    IndexBuffer(const GLuint indices[], const GLsizeiptr size);
    ~IndexBuffer();

    void bind() const;
    void unbind() const;

  private:
    GLuint m_Id;
};
