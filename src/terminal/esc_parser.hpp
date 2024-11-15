#pragma once

#include "terminal_buffer.hpp"
#include "types.hpp"
#include <functional>
#include <optional>
#include <string>
#include <unordered_map>

class EscParser {
    using ident_t = char;
    using handler_t = std::function<void(ParserState&, TerminalBuf&, cursor_t&,
                                         std::optional<std::string>)>;

public:
    void parse(iter_t& it, iter_t end, ParserState& parserState,
               TerminalBuf& termBuf, cursor_t& cursor);
    void addHandler(char ident, handler_t handler, bool acceptsArg = false);

private:
    struct Handler {
        handler_t function;
        bool acceptsArg;
    };

    std::unordered_map<char, Handler> m_Handlers;
};
