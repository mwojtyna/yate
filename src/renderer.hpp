#pragma once

#include "mesh.hpp"
#include "program.hpp"

class Renderer {
  public:
    void draw(const Mesh &mesh, const Program &program) const;
    void setWireframe(const bool enabled) const;
};
