#include "renderer.hpp"
#include "error.hpp"
#include "glad/glad.h"
#include "spdlog/spdlog.h"
#include <cstddef>

void Renderer::draw() const {
    for (size_t i = 0; i < m_Objects.size(); i++) {
        SPDLOG_TRACE("Drawing object with index={}", i);
        m_Objects[i].draw();
    }
}

void Renderer::addObject(Object &object) {
    m_Objects.push_back(object);
}

void Renderer::setWireframe(const bool enabled) const {
    glCall(glPolygonMode(GL_FRONT_AND_BACK, enabled ? GL_LINE : GL_FILL));
}
