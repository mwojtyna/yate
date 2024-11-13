#pragma once

#include "index_buffer.hpp"
#include "opengl.hpp"
#include "program.hpp"
#include "vertex_array.hpp"
#include "vertex_buffer.hpp"
#include <glm/ext/matrix_float4x4.hpp>
#include <vector>

class Mesh {
public:
    Mesh() = delete;
    Mesh(const GLsizei verticesCount, const GLsizei indicesCount,
         const glm::mat4& transform, Program& program);
    ~Mesh();

    void draw(glm::mat4& projection, glm::mat4& view) const;
    void update(std::vector<Vertex>& vertices, std::vector<GLuint>& indices);

private:
    std::unique_ptr<VertexArray> m_Va;
    std::unique_ptr<VertexBuffer> m_Vb;
    std::unique_ptr<IndexBuffer> m_Ib;

    const glm::mat4& m_Transform;
    Program& m_Program;
};
