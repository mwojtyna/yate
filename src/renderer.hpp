#pragma once

#include "glm/ext/matrix_float4x4.hpp"
#include "glm/ext/vector_float3.hpp"
#include "mesh.hpp"
#include "program.hpp"
#include <memory>
#include <string>

class Renderer {
public:
    // TODO: Replace data type with a custom type when parsed terminal output
    void draw(std::string data, glm::mat4& transform, Program& program);
    void setWireframe(const bool enabled) const;
    void setBgColor(const glm::vec3 color);

    static GLfloat LEFT;
    static GLfloat RIGHT;
    static GLfloat BOTTOM;
    static GLfloat TOP;
    static glm::mat4 PROJECTION;
    constexpr static GLfloat SCALE = 30.0f;

private:
    glm::vec3 m_BgColor;
    std::string m_Data;
    std::unique_ptr<Mesh> m_Mesh;
};
