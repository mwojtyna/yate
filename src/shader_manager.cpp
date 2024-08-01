#include "./shader_manager.hpp"
#include "spdlog/spdlog.h"
#include <assert.h>
#include <filesystem>
#include <fstream>

bool ShaderManager::load(std::filesystem::path path) {
    std::ifstream file(path);
    if (!file.is_open()) {
        SPDLOG_ERROR("Failed to open shader file '{}'", path.c_str());
        return false;
    }

    GLuint type;
    if (path.extension() == FRAG_EXTENSION) {
        type = GL_FRAGMENT_SHADER;
    } else if (path.extension() == VERT_EXTENSION) {
        type = GL_VERTEX_SHADER;
    } else {
        SPDLOG_WARN("Shader extension '{}' not recognized",
                    path.extension().c_str());
        return false;
    }

    std::stringstream buffer;
    buffer << file.rdbuf();
    std::string data_string = buffer.str();
    const GLchar *const data = data_string.c_str();

    assert(data_string.length() > 0);

    GLuint shader = glCreateShader(type);
    glShaderSource(shader, 1, &data, nullptr);
    glCompileShader(shader);

    GLint compiled;
    glGetShaderiv(shader, GL_COMPILE_STATUS, &compiled);
    if (compiled != GL_TRUE) {
        GLchar message[ShaderManager::LOG_LEN];
        glGetShaderInfoLog(shader, ShaderManager::LOG_LEN, 0, message);
        SPDLOG_ERROR("Shader '{}' failed to compile:\n{}", path.c_str(),
                     message);
        return false;
    }

    SPDLOG_DEBUG("Compiled shader '{}'", path.c_str());
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

    SPDLOG_DEBUG("Linked shader program");

    return true;
}
