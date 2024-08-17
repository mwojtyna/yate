#pragma once

#include "glad/glad.h"
#include "glm/ext/matrix_float4x4.hpp"
#include "index_buffer.hpp"
#include "program.hpp"
#include "vertex_array.hpp"
#include "vertex_buffer.hpp"
#include <vector>

class Mesh {
public:
    Mesh() = delete;
    Mesh(const GLsizei verticesCount, const GLsizei indicesCount,
         glm::mat4& transform, Program& program);
    ~Mesh();

    void draw() const;
    void update(std::vector<Vertex>& vertices, std::vector<GLuint>& indices);

private:
    std::unique_ptr<VertexArray> m_Va;
    std::unique_ptr<VertexBuffer> m_Vb;
    std::unique_ptr<IndexBuffer> m_Ib;

    glm::mat4& m_Transform;
    Program& m_Program;
};
