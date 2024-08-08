#include "renderer.hpp"
#include "error.hpp"
#include "glad/glad.h"

void Renderer::setWireframe(const bool enabled) const {
    glCall(glPolygonMode(GL_FRONT_AND_BACK, enabled ? GL_LINE : GL_FILL));
}
