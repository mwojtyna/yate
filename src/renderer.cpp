#include "./renderer.hpp"
#include "error.hpp"

void Renderer::render() {
    glCall(glClearColor(0.2f, 0.3f, 0.3f, 1.0f));
    glCall(glClear(GL_COLOR_BUFFER_BIT));

    glCall(glDrawElements(GL_TRIANGLES, 6, GL_UNSIGNED_INT, 0));
}
