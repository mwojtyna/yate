#pragma once

#include "glm/ext/matrix_float4x4.hpp"
#include "glm/ext/vector_float3.hpp"
#include "mesh.hpp"
#include "program.hpp"
#include <string>

class Renderer {
public:
    Renderer() = delete;
    Renderer(Renderer& renderer) = delete;
    Renderer(Renderer&& renderer) = delete;

    static void initialize();
    static void destroy();

    // TODO: Replace data type with a custom type when parsed terminal output
    static void draw(std::string data, glm::mat4& transform, Program& program);
    static void setWireframe(const bool enabled);
    static void setBgColor(const glm::vec3 color);

    static glm::mat4 projection;
    static glm::mat4 view;
    static GLfloat scale;

private:
    static glm::vec3 m_BgColor;
    static std::string m_Data;
    static Mesh* m_CharMesh;
};