#include "imgui_impl_glfw.h"

class DebugUI {
public:
    DebugUI(GLFWwindow* window);
    ~DebugUI();

    void draw();

private:
    bool m_ShowDemoWindow = false;
};
