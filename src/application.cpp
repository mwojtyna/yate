#include "glad/glad.h"
// GLFW (include after glad)
#include "GLFW/glfw3.h"

#include "application.hpp"
#include "error.hpp"
#include "mesh.hpp"
#include "program.hpp"
#include "shaders/fragment.frag.hpp"
#include "shaders/vertex.vert.hpp"
#include "spdlog/cfg/env.h"
#include "spdlog/spdlog.h"
#include "vertex_buffer.hpp"

bool Application::start() const {
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

    const Vertex vertices[] = {
        {{0.5f, 0.5f, 0.0f}, {1.0f, 0.0f, 0.0f, 1.0f}},   // top right
        {{0.5f, -0.5f, 0.0f}, {0.0f, 1.0f, 0.0f, 1.0f}},  // bottom right
        {{-0.5f, -0.5f, 0.0f}, {0.0f, 0.0f, 1.0f, 1.0f}}, // bottom left
        {{-0.5f, 0.5f, 0.0f}, {0.0f, 0.0f, 0.0f, 1.0f}}}; // top left
    const GLuint indices[] = {
        // note that we start from 0!
        0, 1, 3, // first triangle
        1, 2, 3  // second triangle
    };

    const Program program = ProgramBuilder()
                                .loadShader(vertexShader, GL_VERTEX_SHADER)
                                .loadShader(fragmentShader, GL_FRAGMENT_SHADER)
                                .build();
    Mesh mesh("quad", vertices, sizeof(vertices), indices, sizeof(indices));

    m_Renderer.setWireframe(false);

    SPDLOG_INFO("Application started");
    while (!glfwWindowShouldClose(window)) {
        glCall(glClearColor(0.2f, 0.3f, 0.3f, 1.0f));
        glCall(glClear(GL_COLOR_BUFFER_BIT));

        m_Renderer.draw(mesh, program);

        glfwSwapBuffers(window);
        glfwPollEvents();
    }

    return true;
}

Application::~Application() {
    SPDLOG_INFO("Application exiting");
    glfwTerminate();
}
