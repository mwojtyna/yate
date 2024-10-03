#include "codes.hpp"
#include "csi_parser.hpp"
#include "osc_parser.hpp"
#include "parser.hpp"
#include "terminal.hpp"
#include "terminal_buffer.hpp"
#include <GLFW/glfw3.h>
#include <spdlog/spdlog.h>
#include <string>

// CSI

/// Erase from the cursor through the end of the row.
static void eraseToEnd() {
    Terminal::getBufMut([](TerminalBuf& termBuf) {
        termBuf.getRow(termBuf.getRows().size() - 1).pop_back();
    });
}
/// Erase from the beginning of the line through the cursor.
static void eraseToCursor() {
    //
}
/// Erase complete line.
static void eraseLine() {
    //
}

// OSC
static void setWindowTitle(const char* title, GLFWwindow* window) {
    SPDLOG_DEBUG("Set window title: '{}'", title);
    glfwSetWindowTitle(window, title);
}

Parser parser_setup(GLFWwindow* window) {
    CsiParser csi;
    csi.addHandler(csiidents::EL, [](const std::vector<uint32_t> args) {
        assert(args.size() == 0 || args.size() == 1);
        uint32_t ps = args.size() > 0 ? args[0] : 0;
        switch (ps) {
        case 0: {
            eraseToEnd();
            break;
        }
            // case 1: {
            //     eraseToCursor();
            //     break;
            // }
            // case 2: {
            //     eraseLine();
            //     break;
            // }
        }
    });

    OscParser osc;
    osc.addHandler(0, [window](std::vector<std::string> data) {
        setWindowTitle(data[0].c_str(), window);
    });
    osc.addHandler(2, [window](std::vector<std::string> data) {
        setWindowTitle(data[0].c_str(), window);
    });

    Parser parser(std::move(csi), std::move(osc));

    return parser;
}
