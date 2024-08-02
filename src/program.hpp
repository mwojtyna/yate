#pragma once

#include "glad/glad.h"
#include <string>
#include <vector>

class Program {
  public:
    bool load_shader(const GLchar *const data, const GLuint type);
    GLuint link();

  private:
    const static GLuint LOG_LEN = 1024;
    constexpr const static std::string FRAG_EXTENSION = ".frag";
    constexpr const static std::string VERT_EXTENSION = ".vert";

    std::vector<GLuint> shaders;
};
