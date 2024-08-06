#include "program.hpp"
#include "error.hpp"
#include "spdlog/spdlog.h"
#include <assert.h>
#include <cstring>

Program::Program(GLuint id) : m_Id(id){};

Program::~Program() {
    glCall(glDeleteProgram(m_Id));
    SPDLOG_DEBUG("Deleted shader program with id={}", m_Id);
}

void Program::use() const {
    glCall(glUseProgram(m_Id));
    SPDLOG_TRACE("Using shader program with id={}", m_Id);
}

ProgramBuilder &ProgramBuilder::loadShader(const std::string contents,
                                           const GLuint type) {
    assert(contents.length() > 0);
    const GLchar *const string = contents.c_str();

    glCall(GLuint shader = glCreateShader(type));
    glCall(glShaderSource(shader, 1, &string, nullptr));
    glCall(glCompileShader(shader));

    GLint compiled;
    glCall(glGetShaderiv(shader, GL_COMPILE_STATUS, &compiled));
    if (compiled != GL_TRUE) {
        GLchar message[ProgramBuilder::LOG_LEN];
        glCall(glGetShaderInfoLog(shader, ProgramBuilder::LOG_LEN, 0, message));
        SPDLOG_ERROR("Shader with id={} failed to compile:\n{}", shader,
                     message);
        assert(false);
    }

    SPDLOG_DEBUG("Compiled shader with id={}", shader);
    m_Shaders.push_back(shader);

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
    GLchar log[ProgramBuilder::LOG_LEN];
    glCall(glGetProgramiv(programId, GL_LINK_STATUS, &success));
    if (!success) {
        glCall(glGetProgramInfoLog(programId, ProgramBuilder::LOG_LEN, nullptr,
                                   log));
        SPDLOG_ERROR("Shader program linking error:\n{}", log);
        assert(false);
    }
    SPDLOG_DEBUG("Created shader program with id={}", programId);

    for (GLuint shader : m_Shaders) {
        glCall(glDeleteShader(shader));
    }

    return Program(programId);
}
