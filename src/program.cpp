#include "program.hpp"
#include "error.hpp"
#include "glad/glad.h"
#include "glm/ext/matrix_float4x4.hpp"
#include "glm/gtc/type_ptr.hpp"
#include "spdlog/spdlog.h"
#include <assert.h>
#include <cstdlib>

Program::Program(GLuint id) : m_Id(id){};

Program::~Program() {
    glCall(glDeleteProgram(m_Id));
    SPDLOG_DEBUG("Deleted shader program with id={}", m_Id);
}

void Program::use() const {
    glCall(glUseProgram(m_Id));
    SPDLOG_TRACE("Using shader program with id={}", m_Id);
}

void Program::setUniformMatrix4f(const GLchar* const name,
                                 const glm::mat4& mat) {
    use();
    const GLint location = getUniformLocation(name);
    glCall(glUniformMatrix4fv(location, 1, GL_FALSE, glm::value_ptr(mat)));
    SPDLOG_TRACE("Set uniform '{}'", name);
}

ProgramBuilder& ProgramBuilder::loadShader(const std::string contents,
                                           const GLuint type) {
    assert(contents.length() > 0);
    const GLchar* const string = contents.c_str();

    glCall(GLuint shaderId = glCreateShader(type));
    glCall(glShaderSource(shaderId, 1, &string, nullptr));
    glCall(glCompileShader(shaderId));

    GLint compiled;
    glCall(glGetShaderiv(shaderId, GL_COMPILE_STATUS, &compiled));
    if (compiled != GL_TRUE) {
        GLint logLen = 0;
        glCall(glGetShaderiv(shaderId, GL_INFO_LOG_LENGTH, &logLen));

        GLchar* log = (GLchar*)std::malloc(logLen);
        glCall(glGetShaderInfoLog(shaderId, logLen, 0, log));
        SPDLOG_ERROR("Shader with id={} failed to compile:\n{}", shaderId, log);
        std::free(log);
    }

    SPDLOG_DEBUG("Compiled shader with id={}", shaderId);
    m_Shaders.push_back(shaderId);

    return *this;
}

Program ProgramBuilder::build() {
    assert(m_Shaders.size() > 0);

    glCall(GLuint programId = glCreateProgram());
    for (GLuint shader : m_Shaders) {
        glCall(glAttachShader(programId, shader));
    }
    glCall(glLinkProgram(programId));

    GLint success;
    glCall(glGetProgramiv(programId, GL_LINK_STATUS, &success));
    if (!success) {
        GLint logLen = 0;
        glCall(glGetProgramiv(programId, GL_INFO_LOG_LENGTH, &logLen));

        GLchar* log = (GLchar*)std::malloc(logLen);
        glCall(glGetProgramInfoLog(programId, logLen, nullptr, log));
        SPDLOG_ERROR("Shader program linking error:\n{}", log);
        std::free(log);
    }
    SPDLOG_DEBUG("Created shader program with id={}", programId);

    for (GLuint shader : m_Shaders) {
        glCall(glDeleteShader(shader));
    }

    return Program(programId);
}
