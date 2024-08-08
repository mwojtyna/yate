#pragma once

#include "glm/ext/matrix_float4x4.hpp"
#include "mesh.hpp"
#include "program.hpp"

class Object {
public:
    Object() = delete;
    Object(Mesh &mesh, glm::mat4 &transform, Program &program);

    void draw() const;

private:
    glm::mat4 &m_Transform;
    Mesh &m_Mesh;
    Program &m_Program;
};
