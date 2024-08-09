#include "renderer.hpp"
#include "application.hpp"
#include "error.hpp"
#include "glad/glad.h"
#include "glm/ext/matrix_clip_space.hpp"
#include "spdlog/spdlog.h"
#include <cstddef>

glm::mat4 Renderer::PROJECTION =
    glm::ortho(-Application::ASPECT, Application::ASPECT, -1.0f, 1.0f);

void Renderer::draw() const {
    glCall(glClearColor(0.2f, 0.3f, 0.3f, 1.0f));
    glCall(glClear(GL_COLOR_BUFFER_BIT));

    for (size_t i = 0; i < m_Objects.size(); i++) {
        SPDLOG_TRACE("Drawing object with index={}", i);
        m_Objects[i].draw();
    }
}

void Renderer::addObject(Object& object) {
    m_Objects.push_back(object);
}

void Renderer::setWireframe(const bool enabled) const {
    glCall(glPolygonMode(GL_FRONT_AND_BACK, enabled ? GL_LINE : GL_FILL));
}
