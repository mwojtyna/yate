#include "parser_setup.hpp"
#include "csi_idents.hpp"
#include "csi_parser.hpp"
#include "osc_parser.hpp"
#include "terminal.hpp"
#include "terminal_buffer.hpp"
#include "types.hpp"
#include <spdlog/spdlog.h>
#include <string>

#define DEFAULT(arr, default) arr.size() > 0 ? arr[0] : default

// CSI

/// Erase from the cursor through the end of the row.
static void eraseToEnd(cursor_t& cursor) {
    Terminal::getBufMut([&cursor](TerminalBuf& termBuf) {
        std::vector<Cell>& row = termBuf.getRow(cursor.y);
        row.erase(row.begin() + cursor.x, row.end());
    });
}
/// Delete n characters to the left of the cursor
static void deleteCharacters(uint32_t n) {
    Terminal::getBufMut([&n](TerminalBuf& termBuf) {
        cursor_t cursor = Terminal::getCursor();
        std::vector<Cell>& row = termBuf.getRow(cursor.y);
        row.erase(row.begin() + cursor.x, row.begin() + cursor.x + n);
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
            cursor_t cursor = Terminal::getCursor();
            eraseToEnd(cursor);
            break;
        }
        case 1: {
            SPDLOG_WARN("Unimplemented EL(1)");
            break;
        }
        case 2: {
            SPDLOG_WARN("Unimplemented EL(2)");
            break;
        }
        }
    });
    csi.addHandler(csiidents::CUF, [](const std::vector<uint32_t> args) {
        assert(args.size() == 0 || args.size() == 1);
        uint32_t ps = DEFAULT(args, 1);
        Terminal::getCursorMut([&ps](cursor_t& cursor) { cursor.x += ps; });
    });
    csi.addHandler(csiidents::CUB, [](const std::vector<uint32_t> args) {
        assert(args.size() == 0 || args.size() == 1);
        uint32_t ps = DEFAULT(args, 1);
        Terminal::getCursorMut([&ps](cursor_t& cursor) { cursor.x -= ps; });
    });
    csi.addHandler(csiidents::CUP, [](const std::vector<uint32_t> args) {
        assert(args.size() == 0 || args.size() == 2);
        uint32_t x = args.size() == 0 ? 0 : args[0];
        uint32_t y = args.size() == 0 ? 0 : args[1];
        Terminal::getCursorMut([&x, &y](cursor_t& cursor) {
            cursor.x = x;
            cursor.y = y;
        });
    });
    csi.addHandler(csiidents::ED, [](const std::vector<uint32_t> args) {
        assert(args.size() == 0 || args.size() == 1);
        uint32_t ps = DEFAULT(args, 0);
        Terminal::getCursorMut([&ps](cursor_t& cursor) {
            switch (ps) {
            case 0: {
                Terminal::getBufMut([&cursor](TerminalBuf& termBuf) {
                    termBuf.deleteRow(0, termBuf.getRows().size());
                });
                break;
            }
            case 1: {
                SPDLOG_WARN("Unimplemented ED(1)");
                break;
            }
            case 2: {
                Terminal::getBufMut([&cursor](TerminalBuf& termBuf) {
                    termBuf.deleteRow(0, termBuf.getRows().size());
                });
                break;
            }
            case 3: {
                Terminal::getBufMut([&cursor](TerminalBuf& termBuf) {
                    termBuf.deleteRow(0, termBuf.getRows().size());
                });
                break;
            }
            }
        });
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
