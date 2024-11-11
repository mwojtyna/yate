#pragma once

#include "font.hpp"
#include "mesh.hpp"
#include "program.hpp"
#include <glm/ext/matrix_clip_space.hpp>
#include <glm/ext/matrix_float4x4.hpp>
#include <glm/ext/vector_float3.hpp>
#include <memory>

class Renderer {
public:
    void initialize();
    void makeTextMesh(const std::vector<std::vector<Cell>>& cells, Font& font);
    void drawText(const glm::mat4& transform, Program& program);
    void setWireframe(const bool enabled);
    void setBgColor(const glm::vec3& color);
    void clear();
    void setViewMat(const glm::mat4& mat);

    static glm::mat4 getProjectionMat();
    static glm::mat4 getViewMat();

private:
    std::vector<Vertex> m_Vertices;
    std::vector<index_t> m_Indices;
    std::unique_ptr<Mesh> m_GlyphMesh;

    static glm::mat4 s_ProjectionMat;
    static glm::mat4 s_ViewMat;
};
