#pragma once

#include "../error.hpp"
#include <glad/glad.h>
#include <glm/ext/matrix_float4x4.hpp>
#include <spdlog/spdlog.h>
#include <unordered_map>

class Program {
public:
    Program() = delete;
    Program(const std::string vertexShader, const std::string fragmentShader);
    ~Program();

    void use() const;
    void setUniformMatrix4f(const GLchar* const name, const glm::mat4& mat);

private:
    GLuint m_Id = 0;
    /// name -> location
    std::unordered_map<const GLchar*, GLint> m_UniformLocations;

    GLuint loadShader(const std::string contents, const GLuint type) {
        assert(contents.length() > 0);
        const GLchar* const string = contents.c_str();

        glCall(GLuint shaderId = glCreateShader(type));
        glCall(glShaderSource(shaderId, 1, &string, nullptr));
        glCall(glCompileShader(shaderId));

        GLint compiled = GL_FALSE;
        glCall(glGetShaderiv(shaderId, GL_COMPILE_STATUS, &compiled));
        if (compiled != GL_TRUE) {
            GLint logLen = 0;
            glCall(glGetShaderiv(shaderId, GL_INFO_LOG_LENGTH, &logLen));

            GLchar* log = (GLchar*)std::malloc(logLen);
            glCall(glGetShaderInfoLog(shaderId, logLen, 0, log));
            SPDLOG_ERROR("Shader with id={} failed to compile:\n{}", shaderId,
                         log);
            std::free(log);
            std::exit(1);
        }

        SPDLOG_DEBUG("Compiled shader with id={}", shaderId);
        return shaderId;
    }

    GLint getUniformLocation(const GLchar* const name) {
        if (m_UniformLocations.contains(name)) {
            return m_UniformLocations.at(name);
        } else {
            use();
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
