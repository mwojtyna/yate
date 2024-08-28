#pragma once

#include "../application.hpp"
#include "font.hpp"
#include "mesh.hpp"
#include "program.hpp"
#include <glm/ext/matrix_clip_space.hpp>
#include <glm/ext/matrix_float4x4.hpp>
#include <glm/ext/vector_float3.hpp>
#include <memory>

// TODO: Replace data type with a custom type when parsed terminal output
using Code = Codepoint;

struct RendererData {
    glm::mat4 projectionMat = glm::ortho(0.0f, (float)Application::WIDTH,
                                         -(float)Application::HEIGHT, 0.0f);
    glm::mat4 viewMat = glm::mat4(1.0f);
    glm::vec3 bgColor;
    std::vector<Code> codes;
    std::unique_ptr<Mesh> glyphMesh = nullptr;
};

class Renderer {
public:
    Renderer() = delete;
    Renderer(Renderer& renderer) = delete;
    Renderer(Renderer&& renderer) = delete;

    static void initialize();
    static void destroy();

    static void drawText(std::vector<Code>& codes, Font& font,
                         glm::mat4& transform, Program& program);
    static void setWireframe(const bool enabled);
    static void setBgColor(const glm::vec3 color);

    static glm::mat4& getProjectionMat();
    static glm::mat4& getViewMat();
    static void setViewMat(glm::mat4& mat);
    static void setViewMat(glm::mat4&& mat);

private:
    static RendererData* s_Data;
};
