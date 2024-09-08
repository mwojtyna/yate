#include "mesh.hpp"
#include "index_buffer.hpp"
#include "opengl.hpp"
#include "renderer.hpp"
#include "vertex_array.hpp"
#include "vertex_buffer.hpp"
#include <glm/ext/matrix_float4x4.hpp>
#include <spdlog/spdlog.h>

Mesh::Mesh(const GLsizei verticesCount, const GLsizei indicesCount,
           const glm::mat4& transform, Program& program)
    : m_Transform(transform), m_Program(program) {
    m_Va = std::make_unique<VertexArray>();
    m_Ib = std::make_unique<IndexBuffer>(indicesCount);
    m_Vb = std::make_unique<VertexBuffer>(verticesCount);
    m_Va->addBuffer(*m_Vb);

    SPDLOG_DEBUG("Created mesh");
}

Mesh::~Mesh() {
    SPDLOG_DEBUG("Deleted mesh");
}

void Mesh::draw() const {
    glm::mat4 mvp =
        Renderer::getProjectionMat() * Renderer::getViewMat() * m_Transform;
    m_Program.setUniformMatrix4("u_MVP", mvp);

    m_Va->bind();
    glCall(glDrawElements(GL_TRIANGLES, m_Ib->getCount(), GL_UNSIGNED_INT,
                          (const void*)0));
}

void Mesh::update(std::vector<Vertex>& vertices, std::vector<GLuint>& indices) {
    m_Vb->update(vertices);
    m_Ib->update(indices);
}
