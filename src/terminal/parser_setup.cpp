#include "csi_idents.hpp"
#include "csi_parser.hpp"
#include "osc_parser.hpp"
#include "parser.hpp"
#include "terminal.hpp"
#include "terminal_buffer.hpp"
#include "types.hpp"
#include <GLFW/glfw3.h>
#include <spdlog/spdlog.h>
#include <string>

#define DEFAULT(arr, default) arr.size() > 0 ? arr[0] : default

// CSI

/// Erase from the cursor through the end of the row.
static void eraseToEnd() {
    Terminal::getBufMut([](TerminalBuf& termBuf) {
        cursor_t cursor = Terminal::getCursor();
        std::vector<Cell>& row = termBuf.getRow(cursor.y);
        row.erase(row.begin() + cursor.x, row.end());
    });
}
/// Erase from the beginning of the line through the cursor.
static void eraseToCursor() {
    SPDLOG_WARN("Unimplemented EL(1)");
}
/// Erase complete line.
static void eraseLine() {
    SPDLOG_WARN("Unimplemented EL(2)");
}
/// Delete n characters to the left of the cursor
static void deleteCharacters(uint32_t n) {
    Terminal::getBufMut([&n](TerminalBuf& termBuf) {
        cursor_t cursor = Terminal::getCursor();
        std::vector<Cell>& row = termBuf.getRow(cursor.y);
        row.erase(row.begin() + cursor.x + 1 - n, row.begin() + cursor.x + 1);
    });
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
        uint32_t ps = DEFAULT(args, 0);
        switch (ps) {
        case 0: {
            eraseToEnd();
            break;
        }
        case 1: {
            eraseToCursor();
            break;
        }
        case 2: {
            eraseLine();
            break;
        }
        }
    });
    csi.addHandler(csiidents::CUB, [](const std::vector<uint32_t> args) {
        assert(args.size() == 0 || args.size() == 1);
        uint32_t ps = DEFAULT(args, 1);
        Terminal::getCursorMut([&ps](cursor_t& cursor) { cursor.x -= ps; });
    });
    csi.addHandler(csiidents::CUF, [](const std::vector<uint32_t> args) {
        assert(args.size() == 0 || args.size() == 1);
        uint32_t ps = DEFAULT(args, 1);
        Terminal::getCursorMut([&ps](cursor_t& cursor) { cursor.x += ps; });
    });
    csi.addHandler(csiidents::DCH, [](const std::vector<uint32_t> args) {
        assert(args.size() == 0 || args.size() == 1);
        uint32_t ps = DEFAULT(args, 1);
        deleteCharacters(ps);
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
