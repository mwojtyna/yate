#include "glad/glad.h"
// GLFW (include after glad)
#include "GLFW/glfw3.h"

#include "./application.hpp"
#include "shaders/fragment.frag.hpp"
#include "shaders/vertex.vert.hpp"
#include "spdlog/cfg/env.h"
#include "spdlog/spdlog.h"

bool Application::start() {
    spdlog::cfg::load_env_levels();
    SPDLOG_DEBUG("GLFW version: {}", glfwGetVersionString());

    glfwInit();
    glfwWindowHint(GLFW_CONTEXT_VERSION_MAJOR, 3);
    glfwWindowHint(GLFW_CONTEXT_VERSION_MINOR, 3);
    glfwWindowHint(GLFW_OPENGL_PROFILE, GLFW_OPENGL_CORE_PROFILE);

    GLFWwindow *window = glfwCreateWindow(
        Application::WIDTH, Application::HEIGHT, "yate", nullptr, nullptr);
    if (window == nullptr) {
        const char *error;
        glfwGetError(&error);
        SPDLOG_ERROR("Failed to create window: {}", error);
        return false;
    }
    glfwMakeContextCurrent(window);

    if (!gladLoadGLLoader((GLADloadproc)glfwGetProcAddress)) {
        SPDLOG_ERROR("Failed to initialize OpenGL");
        return false;
    } else {
        const GLchar *version = (GLchar *)glGetString(GL_VERSION);
        SPDLOG_DEBUG("OpenGL version: {}", version);
    }

    shader_manager.load(vertex_shader, GL_VERTEX_SHADER);
    shader_manager.load(fragment_shader, GL_FRAGMENT_SHADER);
    if (!shader_manager.link()) {
        return false;
    }

    // TODO: Move to resource manager
    const GLfloat vertices[] = {-0.5f, -0.5f, 0.0f, 0.5f, -0.5f,
                                0.0f,  0.0f,  0.5f, 0.0f};
    GLuint vbo;
    glGenBuffers(1, &vbo);
    glBindBuffer(GL_ARRAY_BUFFER, vbo);
    glBufferData(GL_ARRAY_BUFFER, sizeof(vertices), vertices, GL_STATIC_DRAW);

    glVertexAttribPointer(0, 3, GL_FLOAT, GL_FALSE, 3 * sizeof(GLfloat),
                          (void *)0);
    glEnableVertexAttribArray(0);

    SPDLOG_INFO("Application created");
    while (!glfwWindowShouldClose(window)) {
        renderer.render();
        glfwSwapBuffers(window);
        glfwPollEvents();
    }

    return true;
}

Application::~Application() {
    SPDLOG_INFO("Application exiting");
    glfwTerminate();
}
