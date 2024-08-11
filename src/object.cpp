#include "object.hpp"
#include "renderer.hpp"

Object::Object(Mesh& mesh, glm::mat4& transform, Program& program)
    : m_Transform(transform), m_Mesh(mesh), m_Program(program){};

void Object::draw() const {
    glm::mat4 view = glm::mat4(1.0f);
    glm::mat4 mvp = m_Transform * view * Renderer::PROJECTION;
    m_Program.setUniformMatrix4f("u_MVP", mvp);

    m_Mesh.draw();
}
