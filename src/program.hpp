#pragma once

#include "glad/glad.h"
#include <string>
#include <vector>

class Program {
  public:
    Program() = delete;
    Program(GLuint id);
    ~Program();
    void use() const;

  private:
    const GLuint m_Id;
};

class ProgramBuilder {
  public:
    ProgramBuilder &loadShader(const std::string data, const GLuint type);
    Program build();

  private:
    const static GLuint LOG_LEN = 1024;
    std::vector<GLuint> m_Shaders;
};
