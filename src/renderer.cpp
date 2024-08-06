#include "renderer.hpp"
#include "error.hpp"
#include "program.hpp"

void Renderer::draw(const Mesh &mesh, const Program &program) const {
    program.use();
    mesh.draw();
}

void Renderer::setWireframe(const bool enabled) const {
    glCall(glPolygonMode(GL_FRONT_AND_BACK, enabled ? GL_LINE : GL_FILL));
}
