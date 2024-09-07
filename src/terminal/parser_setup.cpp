#include "../application.hpp"
#include "osc_parser.hpp"
#include "parser.hpp"
#include <GLFW/glfw3.h>
#include <spdlog/spdlog.h>
#include <string>

static void setWindowTitle(const char* title, GLFWwindow* window) {
    SPDLOG_DEBUG("Set window title: '{}'", title);
    glfwSetWindowTitle(window, title);
}

Parser parser_setup(Application& app) {
    OscParser osc;
    osc.addStringHandler(0, [&app](std::vector<std::string> data) {
        setWindowTitle(data[0].c_str(), app.getWindow());
    });

    Parser parser(osc);

    return parser;
}
