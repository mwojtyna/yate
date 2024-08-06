#pragma once

#include "glad/glad.h"
#include <string>
#include <vector>

// TODO: Uniforms

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
    ProgramBuilder &loadShader(const std::string contents, const GLuint type);
    Program build();

  private:
    constexpr static GLuint LOG_LEN = 1024;
    std::vector<GLuint> m_Shaders;
};
