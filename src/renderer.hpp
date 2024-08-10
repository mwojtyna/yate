#pragma once

#include "glm/ext/matrix_float4x4.hpp"
#include "glm/ext/vector_float3.hpp"
#include <string>

class Renderer {
public:
    // TODO: Replace data type with a custom type when parsed terminal output
    void draw(const std::string data) const;
    void setWireframe(const bool enabled) const;
    void setBgColor(const glm::vec3 color);

    static glm::mat4 PROJECTION;

private:
    glm::vec3 m_BgColor;
};
