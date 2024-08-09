#include "object.hpp"
#include "renderer.hpp"

Object::Object(Mesh& mesh, glm::mat4& transform, Program& program)
    : m_Transform(transform), m_Mesh(mesh), m_Program(program) {
    glm::mat4 view = glm::mat4(1.0f);
    glm::mat4 mvp = transform * view * Renderer::PROJECTION;
    program.setUniformMatrix4f("u_MVP", mvp);
};

void Object::draw() const {
    m_Mesh.draw();
}
