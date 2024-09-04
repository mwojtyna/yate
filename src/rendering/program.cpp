#include "program.hpp"
#include "../utils.hpp"
#include "opengl.hpp"
#include <assert.h>
#include <cstdlib>
#include <glm/ext/matrix_float4x4.hpp>
#include <glm/gtc/type_ptr.hpp>
#include <spdlog/spdlog.h>

Program::Program(const std::string vertexShader,
                 const std::string fragmentShader) {
    GLuint vertexShaderId = loadShader(vertexShader, GL_VERTEX_SHADER);
    GLuint fragmentShaderId = loadShader(fragmentShader, GL_FRAGMENT_SHADER);
    GLuint shaders[] = {vertexShaderId, fragmentShaderId};

    glCall(GLuint programId = glCreateProgram());
    m_Id = programId;

    for (GLuint shader : shaders) {
        glCall(glAttachShader(programId, shader));
    }
    glCall(glLinkProgram(programId));

    GLint success = GL_FALSE;
    glCall(glGetProgramiv(programId, GL_LINK_STATUS, &success));
    if (success == GL_FALSE) {
        GLint logLen = 0;
        glCall(glGetProgramiv(programId, GL_INFO_LOG_LENGTH, &logLen));

        GLchar* log = (GLchar*)std::malloc(logLen);
        glCall(glGetProgramInfoLog(programId, logLen, nullptr, log));
        FATAL("Shader program linking error:\n{}", log);
        std::free(log);
    }
    SPDLOG_DEBUG("Created shader program with id={}", programId);

    for (GLuint shader : shaders) {
        glCall(glDeleteShader(shader));
    }
}

Program::~Program() {
    glCall(glDeleteProgram(m_Id));
    SPDLOG_DEBUG("Deleted shader program with id={}", m_Id);
}

void Program::use() const {
    glCall(glUseProgram(m_Id));
    SPDLOG_TRACE("Using shader program with id={}", m_Id);
}

void Program::setUniformFloat(const GLchar* const name, const float value) {
    use();
    const GLint location = getUniformLocation(name);
    glCall(glUniform1f(location, value));
    SPDLOG_TRACE("Set uniform '{}'", name);
}

void Program::setUniformMatrix4(const GLchar* const name,
                                const glm::mat4& mat) {
    use();
    const GLint location = getUniformLocation(name);
    glCall(glUniformMatrix4fv(location, 1, GL_FALSE, glm::value_ptr(mat)));
    SPDLOG_TRACE("Set uniform '{}'", name);
}

// PRIVATE
GLuint Program::loadShader(const std::string contents, const GLuint type) {
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
        FATAL("Shader with id={} failed to compile:\n{}", shaderId, log);
        std::free(log);
    }

    SPDLOG_DEBUG("Compiled shader with id={}", shaderId);
    return shaderId;
}

GLint Program::getUniformLocation(const GLchar* const name) {
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
