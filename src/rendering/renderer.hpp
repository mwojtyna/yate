#pragma once

#include "../application.hpp"
#include "font.hpp"
#include "mesh.hpp"
#include "program.hpp"
#include <glm/ext/matrix_clip_space.hpp>
#include <glm/ext/matrix_float4x4.hpp>
#include <glm/ext/vector_float3.hpp>
#include <memory>

class Renderer {
public:
    Renderer() = delete;
    Renderer(Renderer& renderer) = delete;
    Renderer(Renderer&& renderer) = delete;

    static void initialize();
    static void destroy();

    static void calcText(const std::vector<std::vector<Cell>>& cells,
                         Font& font);
    static void drawText(const glm::mat4& transform, Program& program);
    static void setWireframe(const bool enabled);
    static void setBgColor(const glm::vec3& color);
    static void clear();

    static glm::mat4& getProjectionMat();
    static glm::mat4& getViewMat();
    static void setViewMat(const glm::mat4& mat);

private:
    struct RendererData {
        glm::mat4 projectionMat = glm::ortho(0.0f, (float)Application::WIDTH,
                                             -(float)Application::HEIGHT, 0.0f);
        glm::mat4 viewMat = glm::mat4(1.0f);
        std::vector<Vertex> vertices;
        std::vector<index_t> indices;
        std::unique_ptr<Mesh> glyphMesh;
    };

    static RendererData* s_Data;
};
