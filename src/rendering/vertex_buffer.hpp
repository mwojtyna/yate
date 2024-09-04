#pragma once

#include "opengl.hpp"
#include <glm/ext/vector_float2.hpp>
#include <glm/ext/vector_float3.hpp>
#include <glm/ext/vector_float4.hpp>
#include <vector>

struct Vertex {
    glm::vec3 pos;
    glm::vec4 color;
    glm::vec2 uv;
};

class VertexBuffer {
public:
    VertexBuffer() = delete;
    VertexBuffer(const GLsizei count);
    ~VertexBuffer();

    void bind() const;
    void unbind() const;
    void update(std::vector<Vertex>& vertices);

    GLuint getId() const;

private:
    GLuint m_Id = 0;
};
