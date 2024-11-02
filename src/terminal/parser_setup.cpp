#include "parser_setup.hpp"
#include "../rendering/colors.hpp"
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
static glm::vec4 getColorFromPs(uint32_t ps, bool bg) {
    if (bg) {
        ps -= 40;
    } else {
        ps -= 30;
    }

    switch (ps) {
    case 0: {
        return colors::black;
    }
    case 1: {
        return colors::red;
    }
    case 2: {
        return colors::green;
    }
    case 3: {
        return colors::yellow;
    }
    case 4: {
        return colors::blue;
    }
    case 5: {
        return colors::magenta;
    }
    case 6: {
        return colors::cyan;
    }
    case 7: {
        return colors::white;
    }
    case 9: {
        return bg ? colors::defaultBg : colors::defaultFg;
    }
    default: {
        SPDLOG_WARN("Invalid single color code ({}), returning default color",
                    ps);
        return bg ? colors::defaultBg : colors::defaultFg;
    }
    }
}

// OSC
static void setWindowTitle(const char* title, GLFWwindow* window) {
    SPDLOG_DEBUG("Set window title: '{}'", title);
    glfwSetWindowTitle(window, title);
}

Parser parser_setup(GLFWwindow* window) {
    CsiParser csi;
    csi.addHandler(csiidents::CUU, [](const std::vector<uint32_t> args,
                                      ParserState& parserState) {
        assert(args.size() == 0 || args.size() == 1);
        uint32_t ps = DEFAULT(args, 1);
        Terminal::getCursorMut([&ps](cursor_t& cursor) {
            cursor.y = std::max<size_t>(0, cursor.y - ps);
        });
    });
    csi.addHandler(csiidents::CUD, [](const std::vector<uint32_t> args,
                                      ParserState& parserState) {
        assert(args.size() == 0 || args.size() == 1);
        uint32_t ps = DEFAULT(args, 1);
        Terminal::getCursorMut([&ps](cursor_t& cursor) {
            Terminal::getBufMut([&ps, &cursor](TerminalBuf& termBuf) {
                while (cursor.y + ps > termBuf.getRows().size() - 1) {
                    termBuf.pushRow({Cell::empty()});
                }
            });
            cursor.y += ps;
        });
    });
    csi.addHandler(csiidents::EL, [](const std::vector<uint32_t> args,
                                     ParserState& parserState) {
        assert(args.size() == 0 || args.size() == 1);
        uint32_t ps = DEFAULT(args, 0);
        switch (ps) {
        // Erase from the cursor through the end of the row.
        case 0: {
            cursor_t cursor = Terminal::getCursor();
            Terminal::getBufMut([&cursor](TerminalBuf& termBuf) {
                std::vector<Cell>& row = termBuf.getRow(cursor.y);
                row.erase(row.begin() + cursor.x, row.end());
            });
            break;
        }
        // Erase complete line.
        case 2: {
            Terminal::getBufMut([](TerminalBuf& termBuf) {
                cursor_t cursor = Terminal::getCursor();
                termBuf.deleteRow(cursor.y);
            });
            break;
        }
        default: {
            SPDLOG_WARN("Unimplemented EL({})", ps);
            break;
        }
        }
    });
    csi.addHandler(csiidents::CUF, [](const std::vector<uint32_t> args,
                                      ParserState& parserState) {
        assert(args.size() == 0 || args.size() == 1);
        uint32_t ps = DEFAULT(args, 1);
        Terminal::getCursorMut([&ps](cursor_t& cursor) {
            Terminal::getBufMut([&ps, &cursor](TerminalBuf& termBuf) {
                auto& row = termBuf.getRow(cursor.y);
                while (cursor.x + ps > row.size() - 1) {
                    row.push_back(Cell::empty());
                }
            });
            cursor.x += ps;
        });
    });
    csi.addHandler(csiidents::CUB, [](const std::vector<uint32_t> args,
                                      ParserState& parserState) {
        assert(args.size() == 0 || args.size() == 1);
        uint32_t ps = DEFAULT(args, 1);
        Terminal::getCursorMut([&ps](cursor_t& cursor) {
            cursor.x = std::max<size_t>(0, cursor.x - ps);
        });
    });
    csi.addHandler(csiidents::CHA, [](const std::vector<uint32_t> args,
                                      ParserState& parserState) {
        assert(args.size() == 0 || args.size() == 1);
        uint32_t ps = DEFAULT(args, 1);
        Terminal::getCursorMut([&ps](cursor_t& cursor) {
            cursor.x = std::max<size_t>(0, ps - 1);
        });
    });
    csi.addHandler(csiidents::CUP, [](const std::vector<uint32_t> args,
                                      ParserState& parserState) {
        assert(args.size() == 0 || args.size() == 2);
        uint32_t x = args.size() == 0 ? 1 : args[0];
        uint32_t y = args.size() == 0 ? 1 : args[1];
        Terminal::getCursorMut([&x, &y](cursor_t& cursor) {
            cursor.x = std::max<size_t>(0, x - 1);
            cursor.y = std::max<size_t>(0, y - 1);
        });
    });
    csi.addHandler(csiidents::ED, [](const std::vector<uint32_t> args,
                                     ParserState& parserState) {
        assert(args.size() == 0 || args.size() == 1);
        uint32_t ps = DEFAULT(args, 0);
        Terminal::getCursorMut([&ps](cursor_t& cursor) {
            switch (ps) {
            // Erase from the cursor through the end of the viewport.
            case 0: {
                Terminal::getBufMut([&cursor](TerminalBuf& termBuf) {
                    std::vector<Cell>& row = termBuf.getRow(cursor.y);
                    row.erase(row.begin() + cursor.x, row.end());
                    termBuf.deleteRows(cursor.y + 1, termBuf.getRows().size());
                });
                break;
            }
            // Erase complete viewport.
            case 2: {
                Terminal::getBufMut([&cursor](TerminalBuf& termBuf) {
                    termBuf.deleteRows(0, termBuf.getRows().size());
                });
                break;
            }
            // Erase scrollback (same as ED(1) for now)
            case 3: {
                Terminal::getBufMut([&cursor](TerminalBuf& termBuf) {
                    termBuf.deleteRows(0, termBuf.getRows().size());
                });
                break;
            }
            default: {
                SPDLOG_WARN("Unimplemented ED({})", ps);
                break;
            }
            }
        });
    });
    csi.addHandler(csiidents::DCH, [](const std::vector<uint32_t> args,
                                      ParserState& parserState) {
        assert(args.size() == 0 || args.size() == 1);
        uint32_t ps = DEFAULT(args, 1);
        Terminal::getBufMut([&ps](TerminalBuf& termBuf) {
            cursor_t cursor = Terminal::getCursor();
            std::vector<Cell>& row = termBuf.getRow(cursor.y);
            row.erase(row.begin() + cursor.x, row.begin() + cursor.x + ps);
        });
    });
    csi.addHandler(csiidents::SGR, [](const std::vector<uint32_t> args,
                                      ParserState& parserState) {
        assert(args.size() >= 0 && args.size() <= 32);
        uint32_t p0 = DEFAULT(args, 0);

        switch (args.size()) {
        case 0:
        case 1: {
            switch (p0) {
            case 0: {
                parserState.bgColor = colors::defaultBg;
                parserState.fgColor = colors::defaultFg;
                break;
            }
            default: {
                if (p0 >= 30 && p0 <= 39 && p0 != 38) {
                    parserState.fgColor = getColorFromPs(p0, false);
                } else if (p0 >= 40 && p0 <= 49 && p0 != 48) {
                    parserState.bgColor = getColorFromPs(p0, true);
                } else {
                    SPDLOG_WARN("Unimplemented SGR({})", p0);
                }
                break;
            }
            }
            break;
        }
        case 3: {
            switch (p0) {
            case 38: {
                switch (args[1]) {
                case 5: {
                    parserState.fgColor = colors::colors256[args[2]];
                    break;
                }
                default: {
                    SPDLOG_WARN("Unimplemented SGR({})", vectorToString(args));
                    break;
                }
                }
                break;
            }
            case 48: {
                switch (args[1]) {
                case 5: {
                    parserState.bgColor = colors::colors256[args[2]];
                    break;
                }
                default: {
                    SPDLOG_WARN("Unimplemented SGR({})", vectorToString(args));
                    break;
                }
                }
                break;
            }
            default: {
                SPDLOG_WARN("Unimplemented SGR({})", vectorToString(args));
                break;
            }
            }
            break;
        }
        default: {
            SPDLOG_WARN("Unimplemented SGR({})", vectorToString(args));
            break;
        }
        }
    });

    OscParser osc;
    osc.addHandler(0, [&window](std::vector<std::string> data) {
        setWindowTitle(data[0].c_str(), window);
    });
    osc.addHandler(2, [&window](std::vector<std::string> data) {
        setWindowTitle(data[0].c_str(), window);
    });

    Parser parser(std::move(csi), std::move(osc));

    return parser;
}
