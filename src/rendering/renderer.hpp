#pragma once

#include "font.hpp"
#include "mesh.hpp"
#include "program.hpp"
#include <SDL.h>
#include <glm/ext/matrix_clip_space.hpp>
#include <glm/ext/matrix_float4x4.hpp>
#include <glm/ext/vector_float3.hpp>
#include <memory>

class Renderer {
public:
    Renderer(SDL_Window* window, float contentScale);

    void makeTextMesh(const std::vector<std::vector<Cell>>& cells, Font& font);
    void drawText(const glm::mat4& transform, Program& program);
    void setWireframe(const bool enabled);
    void setBgColor(const glm::vec3& color);
    void clear();
    SDL_GLContext getContext() const;
    void setViewMat(const glm::mat4& mat);

private:
    std::vector<Vertex> m_Vertices;
    std::vector<index_t> m_Indices;
    std::unique_ptr<Mesh> m_GlyphMesh;
    SDL_GLContext m_GlContext;
    glm::mat4 m_ProjectionMat;
    glm::mat4 m_ViewMat;
    float m_ContentScale;
};
