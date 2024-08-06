#pragma once

#include "glad/glad.h"
#include "vertex_buffer.hpp"

enum VertexArrayLayout {
    /// (float X, Y, Z)
    POSITION,
};

class VertexArray {
  public:
    VertexArray();
    ~VertexArray();

    void addBuffer(const VertexBuffer &vb,
                   const VertexArrayLayout layout) const;
    void bind() const;
    void unbind() const;

  private:
    GLuint m_Id;
    VertexArrayLayout m_Layout;
};
