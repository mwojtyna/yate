#include "glad/glad.h"
#include "spdlog/spdlog.h"

bool checkError() {
    while (GLenum error = glGetError()) {
        SPDLOG_ERROR("[OpenGL error] ({0:x})", error);
        return false;
    }
    return true;
}

void clearError() {
    // Skip all errors
    while (glGetError() != GL_NO_ERROR)
        ;
}
