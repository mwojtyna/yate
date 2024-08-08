#include "object.hpp"

Object::Object(Mesh &mesh, glm::mat4 &transform, Program &program)
    : m_Transform(transform), m_Mesh(mesh), m_Program(program) {
    m_Program.setUniformMatrix4f("u_Transform", m_Transform);
};

void Object::draw() {
    m_Mesh.draw();
}
