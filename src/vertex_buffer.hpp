#pragma once

#include "glad/glad.h"

class VertexBuffer {
  public:
    VertexBuffer() = delete;
    VertexBuffer(const void *data, const GLsizeiptr size);
    ~VertexBuffer();

    void bind() const;
    void unbind() const;

    GLuint getId() const;

  private:
    GLuint m_Id;
};
