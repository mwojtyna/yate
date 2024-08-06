#include "renderer.hpp"
#include "program.hpp"

void Renderer::draw(Mesh &mesh, Program &program) const {
    program.use();
    mesh.draw();
}
