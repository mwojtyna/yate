#include "glad/glad.h"
// GLFW (include after glad)
#include "GLFW/glfw3.h"

#include "application.hpp"
#include "debug_ui.hpp"
#include "error.hpp"
#include "glm/ext/matrix_float4x4.hpp"
#include "glm/ext/matrix_transform.hpp"
#include "glm/ext/vector_float3.hpp"
#include "program.hpp"
#include "renderer.hpp"
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

    GLFWwindow* window = glfwCreateWindow(
        Application::WIDTH, Application::HEIGHT, "yate", nullptr, nullptr);
    if (window == nullptr) {
        const char* error;
        glfwGetError(&error);
        SPDLOG_ERROR("Failed to create window: {}", error);
        return false;
    }
    glfwMakeContextCurrent(window);

    if (!gladLoadGLLoader((GLADloadproc)glfwGetProcAddress)) {
        SPDLOG_ERROR("Failed to initialize OpenGL");
        return false;
    } else {
        glCall(const GLchar* version = (GLchar*)glGetString(GL_VERSION));
        SPDLOG_DEBUG("OpenGL version: {}", version);
    }

    DebugUI debugUI(window);

    Program program = ProgramBuilder()
                          .loadShader(vertexShader, GL_VERTEX_SHADER)
                          .loadShader(fragmentShader, GL_FRAGMENT_SHADER)
                          .build();

    m_Renderer.setBgColor(glm::vec3(0.2f, 0.3f, 0.3f));
    m_Renderer.setWireframe(false);

    glm::vec3 translation(0.0f);

    SPDLOG_INFO("Application started");
    while (!glfwWindowShouldClose(window)) {
        glm::mat4 transform = glm::translate(glm::mat4(1.0f), translation);
        m_Renderer.draw("Hello world!", transform, program);

        debugUI.draw(translation);

        glfwSwapBuffers(window);
        glfwPollEvents();
    }

    return true;
}

Application::~Application() {
    SPDLOG_INFO("Application exiting");
    glfwTerminate();
}
