#pragma once

#include "mesh.hpp"
#include "program.hpp"

class Renderer {
  public:
    void draw(Mesh &mesh, Program &program) const;
};
