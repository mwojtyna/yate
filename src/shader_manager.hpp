#pragma once

#include "glad/glad.h"
#include <string>
#include <vector>

class ShaderManager {
  public:
    bool load(const GLchar *const data, const GLuint type);
    bool link();

  private:
    const static GLuint LOG_LEN = 1024;
    constexpr const static std::string FRAG_EXTENSION = ".frag";
    constexpr const static std::string VERT_EXTENSION = ".vert";

    std::vector<GLuint> shaders;
};
