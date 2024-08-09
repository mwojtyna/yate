#include "glad/glad.h"
// GLFW (include after glad)
#include "GLFW/glfw3.h"

#include "application.hpp"
#include "error.hpp"
#include "glm/ext/matrix_float4x4.hpp"
#include "glm/ext/matrix_transform.hpp"
#include "glm/ext/vector_float3.hpp"
#include "mesh.hpp"
#include "object.hpp"
#include "program.hpp"
#include "renderer.hpp"
#include "shaders/fragment.frag.hpp"
#include "shaders/vertex.vert.hpp"
#include "spdlog/cfg/env.h"
#include "spdlog/spdlog.h"
#include "vertex_buffer.hpp"

bool Application::start() {
    spdlog::cfg::load_env_levels();
    SPDLOG_DEBUG("GLFW version: {}", glfwGetVersionString());

    glfwInit();
    glfwWindowHint(GLFW_CONTEXT_VERSION_MAJOR, 3);
    glfwWindowHint(GLFW_CONTEXT_VERSION_MINOR, 3);
    glfwWindowHint(GLFW_OPENGL_PROFILE, GLFW_OPENGL_CORE_PROFILE);
    glfwWindowHint(GLFW_RESIZABLE, GLFW_FALSE);

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

    // TODO: batching
    Program program = ProgramBuilder()
                          .loadShader(vertexShader, GL_VERTEX_SHADER)
                          .loadShader(fragmentShader, GL_FRAGMENT_SHADER)
                          .build();
    glm::mat4 transform = glm::scale(glm::mat4(1.0f), glm::vec3(1.0f));
    Mesh mesh(vertices, sizeof(vertices), indices, sizeof(indices));
    Object quad(mesh, transform, program);
    m_Renderer.addObject(quad);

    m_Renderer.setWireframe(false);

    SPDLOG_INFO("Application started");
    while (!glfwWindowShouldClose(window)) {
        glCall(glClearColor(0.2f, 0.3f, 0.3f, 1.0f));
        glCall(glClear(GL_COLOR_BUFFER_BIT));

        m_Renderer.draw();

        glfwSwapBuffers(window);
        glfwPollEvents();
    }

    return true;
}

Application::~Application() {
    SPDLOG_INFO("Application exiting");
    glfwTerminate();
}
