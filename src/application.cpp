#include "glad/glad.h"
// GLFW (include after glad)
#include "GLFW/glfw3.h"

#include "./application.hpp"
#include "./error.hpp"
#include "program.hpp"
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
        glCall(const GLchar *version = (GLchar *)glGetString(GL_VERSION));
        SPDLOG_DEBUG("OpenGL version: {}", version);
    }

    Program mainProgram;
    mainProgram.loadShader(vertex_shader, GL_VERTEX_SHADER);
    mainProgram.loadShader(fragment_shader, GL_FRAGMENT_SHADER);
    if (mainProgram.link()) {
        mainProgram.use();
    } else {
        return false;
    }

    // TODO: Move to resource manager/renderer
    const GLfloat vertices[] = {
        0.5f,  0.5f,  0.0f, // top right
        0.5f,  -0.5f, 0.0f, // bottom right
        -0.5f, -0.5f, 0.0f, // bottom left
        -0.5f, 0.5f,  0.0f  // top left
    };
    const GLuint indices[] = {
        // note that we start from 0!
        0, 1, 3, // first triangle
        1, 2, 3  // second triangle
    };

    GLuint vao;
    glCall(glGenVertexArrays(1, &vao));
    glCall(glBindVertexArray(vao));

    GLuint vbo;
    glCall(glGenBuffers(1, &vbo));
    glCall(glBindBuffer(GL_ARRAY_BUFFER, vbo));
    glCall(glBufferData(GL_ARRAY_BUFFER, sizeof(vertices), vertices,
                        GL_STATIC_DRAW));

    GLuint ebo;
    glCall(glGenBuffers(1, &ebo));
    glCall(glBindBuffer(GL_ELEMENT_ARRAY_BUFFER, ebo));
    glCall(glBufferData(GL_ELEMENT_ARRAY_BUFFER, sizeof(indices), indices,
                        GL_STATIC_DRAW));

    glCall(glVertexAttribPointer(0, 3, GL_FLOAT, GL_FALSE, 3 * sizeof(GLfloat),
                                 (void *)0));
    glCall(glEnableVertexAttribArray(0));

    glCall(glPolygonMode(GL_FRONT_AND_BACK, GL_LINE));

    SPDLOG_INFO("Application started");
    while (!glfwWindowShouldClose(window)) {
        m_Renderer.render();

        glfwSwapBuffers(window);
        glfwPollEvents();
    }

    return true;
}

Application::~Application() {
    SPDLOG_INFO("Application exiting");
    glfwTerminate();
}
