#pragma once

#include "glad/glad.h"
#include "glm/ext/matrix_float4x4.hpp"
#include "index_buffer.hpp"
#include "program.hpp"
#include "vertex_array.hpp"
#include "vertex_buffer.hpp"
#include <memory>

class Mesh {
public:
    Mesh() = delete;
    Mesh(const Vertex vertices[], const GLsizei verticesCount,
         const GLuint indices[], const GLsizei indicesCount,
         glm::mat4& transform, Program& program);
    ~Mesh();
    void draw() const;

private:
    std::unique_ptr<const VertexArray> m_Va;
    std::unique_ptr<const VertexBuffer> m_Vb;
    std::unique_ptr<const IndexBuffer> m_Ib;
    const GLsizei m_IndicesCount = 0;
    glm::mat4& m_Transform;
    Program& m_Program;
};