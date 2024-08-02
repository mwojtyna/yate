#include "./program.hpp"
#include "./error.hpp"
#include "spdlog/spdlog.h"
#include <assert.h>
#include <cstring>

bool Program::load_shader(const GLchar *const data, const GLuint type) {
    assert(std::strlen(data) > 0);

    glCall(GLuint shader = glCreateShader(type));
    glCall(glShaderSource(shader, 1, &data, nullptr));
    glCall(glCompileShader(shader));

    GLint compiled;
    glCall(glGetShaderiv(shader, GL_COMPILE_STATUS, &compiled));
    if (compiled != GL_TRUE) {
        GLchar message[Program::LOG_LEN];
        glCall(glGetShaderInfoLog(shader, Program::LOG_LEN, 0, message));
        SPDLOG_ERROR("Shader with id {} failed to compile:\n{}", shader,
                     message);
        return false;
    }

    SPDLOG_DEBUG("Compiled shader with id {}", shader);
    shaders.push_back(shader);

    return true;
}

GLuint Program::link() {
    assert(shaders.size() > 0);

    glCall(GLuint program = glCreateProgram());
    for (GLuint shader : shaders) {
        glCall(glAttachShader(program, shader));
    }
    glCall(glLinkProgram(program));

    GLint success;
    GLchar log[512];
    glCall(glGetProgramiv(program, GL_LINK_STATUS, &success));
    if (!success) {
        glCall(glGetProgramInfoLog(program, 512, nullptr, log));
        SPDLOG_ERROR("Shader program linking error:\n{}", log);
        return 0;
    }

    for (GLuint shader : shaders) {
        glCall(glDeleteShader(shader));
    }
    shaders.clear();

    SPDLOG_DEBUG("Linked shader program");

    return program;
}
