#pragma once

#include "glad/glad.h"
#include <vector>

class Program {
  public:
    bool loadShader(const GLchar *const data, const GLuint type);
    bool link();
    void use();
    ~Program();

  private:
    const static GLuint LOG_LEN = 1024;

    std::vector<GLuint> m_Shaders;
    GLuint m_Program;
};
