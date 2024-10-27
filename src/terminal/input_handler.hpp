#pragma once

#include <GLFW/glfw3.h>

class InputHandler {
public:
    InputHandler(GLFWwindow* window);
    void setupHandlers();

private:
    GLFWwindow* m_Window;
};
