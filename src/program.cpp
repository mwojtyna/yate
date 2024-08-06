#include "program.hpp"
#include "error.hpp"
#include "spdlog/spdlog.h"
#include <assert.h>
#include <cstring>
#include <string>

bool Program::loadShader(const std::string contents, const GLuint type) {
    assert(contents.length() > 0);
    const GLchar *const string = contents.c_str();

    glCall(GLuint shader = glCreateShader(type));
    glCall(glShaderSource(shader, 1, &string, nullptr));
    glCall(glCompileShader(shader));

    GLint compiled;
    glCall(glGetShaderiv(shader, GL_COMPILE_STATUS, &compiled));
    if (compiled != GL_TRUE) {
        GLchar message[Program::LOG_LEN];
        glCall(glGetShaderInfoLog(shader, Program::LOG_LEN, 0, message));
        SPDLOG_ERROR("Shader with id={} failed to compile:\n{}", shader,
                     message);
        return false;
    }

    SPDLOG_DEBUG("Compiled shader with id={}", shader);
    m_Shaders.push_back(shader);

    return true;
}

bool Program::link() {
    assert(m_Shaders.size() > 0);

    glCall(GLuint program = glCreateProgram());
    for (GLuint shader : m_Shaders) {
        glCall(glAttachShader(program, shader));
    }
    glCall(glLinkProgram(program));

    GLint success;
    GLchar log[LOG_LEN];
    glCall(glGetProgramiv(program, GL_LINK_STATUS, &success));
    if (!success) {
        glCall(glGetProgramInfoLog(program, LOG_LEN, nullptr, log));
        SPDLOG_ERROR("Shader program linking error:\n{}", log);
        return false;
    }
    SPDLOG_TRACE("Linked shader program with id={}", program);

    for (GLuint shader : m_Shaders) {
        glCall(glDeleteShader(shader));
    }
    m_Shaders.clear();

    m_Id = program;

    return true;
}

void Program::use() const {
    glCall(glUseProgram(m_Id));
    SPDLOG_TRACE("Using shader program with id={}", m_Id);
}

Program::~Program() {
    glCall(glDeleteProgram(m_Id));
    SPDLOG_DEBUG("Deleted shader program with id={}", m_Id);
}
