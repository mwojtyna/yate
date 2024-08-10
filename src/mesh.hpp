#pragma once

#include "glad/glad.h"
#include "index_buffer.hpp"
#include "vertex_array.hpp"
#include "vertex_buffer.hpp"
#include <memory>

class Mesh {
public:
    Mesh() = delete;
    Mesh(const Vertex vertices[], const GLsizei verticesCount,
         const GLuint indices[], const GLsizei indicesCount);
    ~Mesh();
    void draw() const;

private:
    std::unique_ptr<const VertexArray> m_Va;
    std::unique_ptr<const VertexBuffer> m_Vb;
    std::unique_ptr<const IndexBuffer> m_Ib;
    const GLsizei m_IndicesCount = 0;
};
