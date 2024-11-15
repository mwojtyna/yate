#include "parser_setup.hpp"
#include "../rendering/colors.hpp"
#include "csi_idents.hpp"
#include "csi_parser.hpp"
#include "esc_parser.hpp"
#include "osc_parser.hpp"
#include "terminal_buffer.hpp"
#include "types.hpp"
#include <spdlog/spdlog.h>
#include <string>

#define DEFAULT(arr, default) arr.size() > 0 ? arr[0] : default

// CSI
static glm::vec4 getSystemColorFromPs(uint32_t ps, bool bg) {
    if (bg) {
        ps -= 40;
    } else {
        ps -= 30;
    }

    if (ps >= 0 && ps <= 7) {
        return colors::colors256[ps];
    } else if (ps == 9) {
        return bg ? colors::defaultBg : colors::defaultFg;
    } else {
        SPDLOG_WARN(
            "Invalid single system color code ({}), returning default color",
            ps);
        return bg ? colors::defaultBg : colors::defaultFg;
    }
}
static glm::vec4 getBrightColorFromPs(uint32_t ps, bool bg) {
    if (bg) {
        ps -= 100;
    } else {
        ps -= 90;
    }

    if (ps >= 0 && ps <= 7) {
        return colors::colors256[ps + 8];
    } else {
        SPDLOG_WARN(
            "Invalid single bright color code ({}), returning default color",
            ps);
        return bg ? colors::defaultBg : colors::defaultFg;
    }
}
static void cursorForward(const uint32_t ps, TerminalBuf& termBuf,
                          cursor_t& cursor) {
    auto& row = termBuf.getRow(cursor.y);
    while (cursor.x + ps + 1 > row.size()) {
        row.push_back(Cell::empty());
    }
    cursor.x += ps;
}
/// Argument must start at 0
static void setCursorX(uint32_t x, cursor_t& cursor) {
    cursor.x = std::max<float>(0, x);
}
/// Arguments must start at 0
static void setCursor(uint32_t x, uint32_t y, TerminalBuf& termBuf,
                      cursor_t& cursor) {
    x = std::max<float>(0, x);
    y = std::max<float>(0, y);

    while (cursor.y + y + 1 > termBuf.getRows().size()) {
        termBuf.pushRow({});
    }
    auto& row = termBuf.getRow(cursor.y);
    while (cursor.x + x + 1 > row.size()) {
        row.push_back(Cell::empty());
    }

    cursor.x = x;
    cursor.y = y;
}

Parser parser_setup(SDL_Window* window) {
    CsiParser csi;

    csi.addHandler(csiidents::ICH, [](const std::vector<uint32_t> args,
                                      ParserState& parserState,
                                      TerminalBuf& termBuf, cursor_t& cursor) {
        assert(args.size() == 0 || args.size() == 1);
        const uint32_t ps = DEFAULT(args, 1);
        std::vector<Cell>& row = termBuf.getRow(cursor.y);
        row.insert(row.begin() + cursor.x, ps, Cell::empty());
    });
    csi.addHandler(csiidents::CUU, [](const std::vector<uint32_t> args,
                                      ParserState& parserState,
                                      TerminalBuf& termBuf, cursor_t& cursor) {
        assert(args.size() == 0 || args.size() == 1);
        const uint32_t ps = DEFAULT(args, 1);
        cursor.y = std::max<float>(0, cursor.y - ps);
    });
    csi.addHandler(csiidents::CUD, [](const std::vector<uint32_t> args,
                                      ParserState& parserState,
                                      TerminalBuf& termBuf, cursor_t& cursor) {
        assert(args.size() == 0 || args.size() == 1);
        const uint32_t ps = DEFAULT(args, 1);
        while (cursor.y + ps + 1 > termBuf.getRows().size()) {
            termBuf.pushRow({});
        }
        cursor.y += ps;
    });
    csi.addHandler(csiidents::EL, [](const std::vector<uint32_t> args,
                                     ParserState& parserState,
                                     TerminalBuf& termBuf, cursor_t& cursor) {
        assert(args.size() == 0 || args.size() == 1);
        const uint32_t ps = DEFAULT(args, 0);
        switch (ps) {
        // Erase from the cursor through the end of the row.
        case 0: {
            if (!termBuf.getRows().empty()) {
                std::vector<Cell>& row = termBuf.getRow(cursor.y);
                row.erase(row.begin() + cursor.x, row.end());
            }
            break;
        }
        // Erase complete line.
        case 2: {
            if (!termBuf.getRows().empty()) {
                termBuf.deleteRow(cursor.y);
            }
            break;
        }
        default: {
            SPDLOG_WARN("Unimplemented EL({})", ps);
            break;
        }
        }
    });
    csi.addHandler(csiidents::CUF, [](const std::vector<uint32_t> args,
                                      ParserState& parserState,
                                      TerminalBuf& termBuf, cursor_t& cursor) {
        assert(args.size() == 0 || args.size() == 1);
        const uint32_t ps = DEFAULT(args, 1);
        cursorForward(ps, termBuf, cursor);
    });
    csi.addHandler(csiidents::CUB, [](const std::vector<uint32_t> args,
                                      ParserState& parserState,
                                      TerminalBuf& termBuf, cursor_t& cursor) {
        assert(args.size() == 0 || args.size() == 1);
        const uint32_t ps = DEFAULT(args, 1);
        cursor.x = std::max<float>(0, cursor.x - ps);
    });
    csi.addHandler(csiidents::CHA, [](const std::vector<uint32_t> args,
                                      ParserState& parserState,
                                      TerminalBuf& termBuf, cursor_t& cursor) {
        assert(args.size() == 0 || args.size() == 1);
        const uint32_t ps = DEFAULT(args, 1);
        setCursorX(ps - 1, cursor);
    });
    csi.addHandler(csiidents::CUP, [](const std::vector<uint32_t> args,
                                      ParserState& parserState,
                                      TerminalBuf& termBuf, cursor_t& cursor) {
        assert(args.size() == 0 || args.size() == 2);
        uint32_t x = args.size() == 0 ? 1 : args[0];
        uint32_t y = args.size() == 0 ? 1 : args[1];
        setCursor(x - 1, y - 1, termBuf, cursor);
    });
    csi.addHandler(csiidents::ED, [](const std::vector<uint32_t> args,
                                     ParserState& parserState,
                                     TerminalBuf& termBuf, cursor_t& cursor) {
        assert(args.size() == 0 || args.size() == 1);
        const uint32_t ps = DEFAULT(args, 0);
        switch (ps) {
        // Erase from the cursor through the end of the viewport.
        case 0: {
            std::vector<Cell>& row = termBuf.getRow(cursor.y);
            row.erase(row.begin() + cursor.x, row.end());
            termBuf.deleteRows(cursor.y + 1, termBuf.getRows().size());
            break;
        }
        // Erase complete viewport.
        case 2: {
            termBuf.deleteRows(0, termBuf.getRows().size());
            break;
        }
        // Erase scrollback (same as ED(1) for now)
        case 3: {
            termBuf.deleteRows(0, termBuf.getRows().size());
            break;
        }
        default: {
            SPDLOG_WARN("Unimplemented ED({})", ps);
            break;
        }
        }
    });
    csi.addHandler(csiidents::DCH, [](const std::vector<uint32_t> args,
                                      ParserState& parserState,
                                      TerminalBuf& termBuf, cursor_t& cursor) {
        assert(args.size() == 0 || args.size() == 1);
        const uint32_t ps = DEFAULT(args, 1);
        std::vector<Cell>& row = termBuf.getRow(cursor.y);
        row.erase(row.begin() + cursor.x, row.begin() + cursor.x + ps);
    });
    csi.addHandler(csiidents::HPA, [](const std::vector<uint32_t> args,
                                      ParserState& parserState,
                                      TerminalBuf& termBuf, cursor_t& cursor) {
        assert(args.size() == 0 || args.size() == 1);
        const uint32_t ps = DEFAULT(args, 1);
        setCursorX(ps - 1, cursor);
    });
    csi.addHandler(csiidents::HPR, [](const std::vector<uint32_t> args,
                                      ParserState& parserState,
                                      TerminalBuf& termBuf, cursor_t& cursor) {
        assert(args.size() == 0 || args.size() == 1);
        const uint32_t ps = DEFAULT(args, 1);
        cursorForward(ps, termBuf, cursor);
    });
    csi.addHandler(csiidents::HVP, [](const std::vector<uint32_t> args,
                                      ParserState& parserState,
                                      TerminalBuf& termBuf, cursor_t& cursor) {
        assert(args.size() == 0 || args.size() == 2);
        uint32_t x = args.size() == 0 ? 1 : args[0];
        uint32_t y = args.size() == 0 ? 1 : args[1];
        setCursor(x - 1, y - 1, termBuf, cursor);
    });
    csi.addHandler(csiidents::SGR, [](const std::vector<uint32_t> args,
                                      ParserState& parserState,
                                      TerminalBuf& termBuf, cursor_t& cursor) {
        assert(args.size() >= 0 && args.size() <= 32);
        if (args.size() == 0) {
            parserState.bgColor = colors::defaultBg;
            parserState.fgColor = colors::defaultFg;
            return;
        }

        size_t i = 0;
        while (i < args.size()) {
            uint32_t pi = args[i++];
            switch (pi) {
            case 0: {
                parserState.bgColor = colors::defaultBg;
                parserState.fgColor = colors::defaultFg;
                break;
            }
            case 7: {
                parserState.inversed = true;
                break;
            }
            case 27: {
                parserState.inversed = false;
                break;
            }
            case 38: {
                uint8_t extendedColorType = args[i++];
                switch (extendedColorType) {
                case 2: {
                    uint8_t r = args[i++];
                    uint8_t g = args[i++];
                    uint8_t b = args[i++];
                    parserState.fgColor =
                        glm::vec4(r / 255.0, g / 255.0, b / 255.0, 1);
                    break;
                }
                case 5: {
                    parserState.fgColor = colors::colors256[args[i++]];
                    break;
                }
                default: {
                    SPDLOG_WARN("Unimplemented '{}' (index={}) in SGR({})",
                                extendedColorType, i - 1, vectorToString(args));
                    break;
                }
                }
                continue;
            }
            case 48: {
                uint8_t extendedColorType = args[i++];
                switch (extendedColorType) {
                case 2: {
                    uint8_t r = args[i++];
                    uint8_t g = args[i++];
                    uint8_t b = args[i++];
                    parserState.bgColor =
                        glm::vec4(r / 255.0, g / 255.0, b / 255.0, 1);
                    break;
                }
                case 5: {
                    parserState.bgColor = colors::colors256[args[i++]];
                    break;
                }
                default: {
                    SPDLOG_WARN("Unimplemented '{}' (index={}) in SGR({})",
                                extendedColorType, i - 1, vectorToString(args));
                    break;
                }
                }
                continue;
            }
            default: {
                if (pi >= 30 && pi <= 39) {
                    parserState.fgColor = getSystemColorFromPs(pi, false);
                } else if (pi >= 40 && pi <= 49) {
                    parserState.bgColor = getSystemColorFromPs(pi, true);
                } else if (pi >= 90 && pi <= 97) {
                    parserState.fgColor = getBrightColorFromPs(pi, false);
                } else if (pi >= 100 && pi <= 107) {
                    parserState.bgColor = getBrightColorFromPs(pi, true);
                } else {
                    SPDLOG_WARN("Unimplemented '{}' (index={}) in SGR({})", pi,
                                i - 1, vectorToString(args));
                }
                break;
            }
            }
        }
    });

    OscParser osc;
    osc.addHandler(0, [window](const std::vector<std::string> args,
                               TerminalBuf& termBuf, cursor_t& cursor) {
        SDL_SetWindowTitle(window, args[0].c_str());
    });
    osc.addHandler(2, [window](const std::vector<std::string> args,
                               TerminalBuf& termBuf, cursor_t& cursor) {
        SDL_SetWindowTitle(window, args[0].c_str());
    });

    EscParser esc;
    esc.addHandler('7', [](ParserState& parserState, TerminalBuf& termBuf,
                           cursor_t& cursor, std::optional<std::string> arg) {
        parserState.savedCursorData = cursor;
    });
    esc.addHandler('8', [](ParserState& parserState, TerminalBuf& termBuf,
                           cursor_t& cursor, std::optional<std::string> arg) {
        cursor = parserState.savedCursorData;
    });
    esc.addHandler(
        'k',
        [window](ParserState& parserState, TerminalBuf& termBuf,
                 cursor_t& cursor, std::optional<std::string> arg) {
            assert(arg.has_value());
            SDL_SetWindowTitle(window, arg.value().c_str());
        },
        true);
    esc.addHandler('M', [](ParserState& parserState, TerminalBuf& termBuf,
                           cursor_t& cursor, std::optional<std::string> arg) {
        // TODO: Scroll when needed
        cursor.y = std::max<float>(cursor.y - 1, 0);
    });

    Parser parser(std::move(csi), std::move(osc), std::move(esc));

    return parser;
}
