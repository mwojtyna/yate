#pragma once

#include "opengl.hpp"
#include <glm/ext/matrix_float4x4.hpp>
#include <spdlog/spdlog.h>
#include <unordered_map>

class Program {
public:
    Program() = delete;
    Program(const std::string vertexShader, const std::string fragmentShader);
    ~Program();

    void use() const;
    void setUniformFloat(const GLchar* const name, const float value);
    void setUniformMatrix4(const GLchar* const name, const glm::mat4& mat);

private:
    GLuint loadShader(const std::string contents, const GLuint type);
    GLint getUniformLocation(const GLchar* const name);

    GLuint m_Id = 0;
    /// name -> location
    std::unordered_map<const GLchar*, GLint> m_UniformLocations;
};
