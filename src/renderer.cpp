#include "renderer.hpp"
#include "application.hpp"
#include "error.hpp"
#include "glad/glad.h"
#include "glm/ext/matrix_clip_space.hpp"
#include "glm/fwd.hpp"
#include <string>

glm::mat4 Renderer::PROJECTION =
    glm::ortho(-Application::ASPECT, Application::ASPECT, -1.0f, 1.0f);

void Renderer::draw(const std::string data) const {
    glCall(glClearColor(m_BgColor.r, m_BgColor.g, m_BgColor.b, 1.0f));
    glCall(glClear(GL_COLOR_BUFFER_BIT));

    for (auto& c : data) {
        (void)(c);
    }
}

void Renderer::setWireframe(const bool enabled) const {
    glCall(glPolygonMode(GL_FRONT_AND_BACK, enabled ? GL_LINE : GL_FILL));
}

void Renderer::setBgColor(const glm::vec3 color) {
    m_BgColor = color;
}
