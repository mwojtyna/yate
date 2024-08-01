#include "glad/glad.h"
// GLFW (include after glad)
#include "GLFW/glfw3.h"

#include "./application.hpp"
#include "spdlog/cfg/env.h"
#include "spdlog/spdlog.h"

bool Application::start() {
    spdlog::cfg::load_env_levels();
    SPDLOG_DEBUG("GLFW version: {}", glfwGetVersionString());

    glfwInit();
    glfwWindowHint(GLFW_CONTEXT_VERSION_MAJOR, 3);
    glfwWindowHint(GLFW_CONTEXT_VERSION_MINOR, 3);
    glfwWindowHint(GLFW_OPENGL_PROFILE, GLFW_OPENGL_CORE_PROFILE);

    GLFWwindow *window =
        glfwCreateWindow(Application::WIDTH, Application::HEIGHT,
                         Application::TITLE, nullptr, nullptr);
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

    SPDLOG_INFO("Application created");

    while (!glfwWindowShouldClose(window)) {
        glClearColor(0.2f, 0.3f, 0.3f, 1.0f);
        glClear(GL_COLOR_BUFFER_BIT);

        glfwSwapBuffers(window);
        glfwPollEvents();
    }

    return true;
}

Application::~Application() {
    SPDLOG_INFO("Application exiting");
    glfwTerminate();
}
