#include "./shader_manager.hpp"
#include "spdlog/spdlog.h"
#include <assert.h>
#include <cstring>

bool ShaderManager::load(const GLchar *const data, const GLuint type) {
    assert(std::strlen(data) > 0);

    GLuint shader = glCreateShader(type);
    glShaderSource(shader, 1, &data, nullptr);
    glCompileShader(shader);

    GLint compiled;
    glGetShaderiv(shader, GL_COMPILE_STATUS, &compiled);
    if (compiled != GL_TRUE) {
        GLchar message[ShaderManager::LOG_LEN];
        glGetShaderInfoLog(shader, ShaderManager::LOG_LEN, 0, message);
        SPDLOG_ERROR("Shader with id {} failed to compile:\n{}", shader,
                     message);
        return false;
    }

    SPDLOG_DEBUG("Compiled shader with id {}", shader);
    shaders.push_back(shader);

    return true;
}

bool ShaderManager::link() {
    assert(shaders.size() > 0);

    GLuint program = glCreateProgram();
    for (GLuint shader : shaders) {
        glAttachShader(program, shader);
    }
    glLinkProgram(program);

    GLint success;
    GLchar log[512];
    glGetProgramiv(program, GL_LINK_STATUS, &success);
    if (!success) {
        glGetProgramInfoLog(program, 512, nullptr, log);
        SPDLOG_ERROR("Shader program linking error:\n{}", log);
        return false;
    }
    glUseProgram(program);

    for (GLuint shader : shaders) {
        glDeleteShader(shader);
    }
    shaders.clear();

    SPDLOG_DEBUG("Linked shader program");

    return true;
}
