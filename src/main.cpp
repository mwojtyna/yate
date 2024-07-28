#include <GLFW/glfw3.h>
#include <iostream>
#include <ostream>

#ifdef __APPLE__
#define GL_SILENCE_DEPRECATION
#endif

int main() {
    std::cerr << glfwGetVersionString() << std::endl;

    glfwInit();
    glfwWindowHint(GLFW_CONTEXT_VERSION_MAJOR, 3);
    glfwWindowHint(GLFW_CONTEXT_VERSION_MINOR, 3);
    glfwWindowHint(GLFW_OPENGL_PROFILE, GLFW_OPENGL_CORE_PROFILE);

    GLFWwindow *window = glfwCreateWindow(640, 480, "yate", nullptr, nullptr);
    if (window == nullptr) {
        const char *error;
        glfwGetError(&error);
        std::cerr << "Failed to create window: " << error << std::endl;

        glfwTerminate();
        return -1;
    }

    glfwMakeContextCurrent(window);

    while (!glfwWindowShouldClose(window)) {
        glClearColor(0.2f, 0.3f, 0.3f, 1.0f);
        glClear(GL_COLOR_BUFFER_BIT);

        glfwSwapBuffers(window);
        glfwPollEvents();
    }

    glfwTerminate();
    return 0;
}
