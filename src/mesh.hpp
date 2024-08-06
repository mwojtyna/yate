#pragma once

#include "glad/glad.h"
#include "index_buffer.hpp"
#include "vertex_array.hpp"
#include "vertex_buffer.hpp"
#include <memory>
#include <string_view>

class Mesh {
  public:
    Mesh() = delete;
    Mesh(const std::string_view name, const Vertex vertices[],
         const GLsizeiptr verticiesSize, const GLuint indices[],
         const GLsizeiptr indicesSize);
    ~Mesh();

    void draw() const;

  private:
    std::unique_ptr<const VertexArray> m_Va;
    std::unique_ptr<const VertexBuffer> m_Vb;
    std::unique_ptr<const IndexBuffer> m_Ib;
    const GLsizei m_IndicesCount;
    const std::string_view m_Name;
};
