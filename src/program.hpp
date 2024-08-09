#pragma once

#include "error.hpp"
#include "glad/glad.h"
#include "glm/ext/matrix_float4x4.hpp"
#include "spdlog/spdlog.h"
#include <unordered_map>
#include <vector>

class Program {
public:
    Program() = delete;
    Program(GLuint id);
    ~Program();

    void use() const;
    void setUniformMatrix4f(const GLchar* const name, const glm::mat4& mat);

private:
    const GLuint m_Id;
    /// name -> location
    std::unordered_map<const GLchar*, GLint> m_UniformLocations;

    GLint getUniformLocation(const GLchar* const name) {
        use();
        if (m_UniformLocations.contains(name)) {
            return m_UniformLocations.at(name);
        } else {
            glCall(const GLint location = glGetUniformLocation(m_Id, name));
            if (location == -1) {
                SPDLOG_ERROR("Uniform named '{}' not found", name);
                return -1;
            }
            m_UniformLocations[name] = location;
            return location;
        }
    }
};

class ProgramBuilder {
public:
    ProgramBuilder& loadShader(const std::string contents, const GLuint type);
    Program build();

private:
    std::vector<GLuint> m_Shaders;
};
