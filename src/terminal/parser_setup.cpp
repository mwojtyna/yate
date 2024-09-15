#include "csi_parser.hpp"
#include "osc_parser.hpp"
#include "parser.hpp"
#include <GLFW/glfw3.h>
#include <spdlog/spdlog.h>
#include <string>

static void setWindowTitle(const char* title, GLFWwindow* window) {
    SPDLOG_DEBUG("Set window title: '{}'", title);
    glfwSetWindowTitle(window, title);
}

Parser parser_setup(GLFWwindow* window) {
    CsiParser csi;

    OscParser osc;
    osc.addStringHandler(0, [window](std::vector<std::string> data) {
        setWindowTitle(data[0].c_str(), window);
    });
    osc.addStringHandler(2, [window](std::vector<std::string> data) {
        setWindowTitle(data[0].c_str(), window);
    });

    Parser parser(csi, osc);

    return parser;
}
