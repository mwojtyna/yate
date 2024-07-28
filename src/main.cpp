#include <GLFW/glfw3.h>
#include <iostream>
#include <ostream>

int main() {
    std::cout << glfwGetVersionString() << std::flush;

    glfwInit();

    GLFWwindow* window = glfwCreateWindow(640, 480, "yate", nullptr, nullptr);
    if (window == nullptr) {
        glfwTerminate();
        return 1;
    }

    glfwMakeContextCurrent(window);

    while (!glfwWindowShouldClose(window)) {
        glfwSwapBuffers(window);
        glfwPollEvents();
    }

    glfwTerminate();
    return 0;
}
