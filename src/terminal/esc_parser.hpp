#pragma once

#include "terminal_buffer.hpp"
#include "types.hpp"
#include <functional>
#include <string>
#include <unordered_map>

class EscParser {
    using ident_t = char;
    using handler_t =
        std::function<void(ParserState&, TerminalBuf&, cursor_t&)>;
    using handler_with_arg_t =
        std::function<void(ParserState&, TerminalBuf&, cursor_t&, std::string)>;

public:
    void parse(iter_t& it, iter_t end, ParserState& parserState,
               TerminalBuf& termBuf, cursor_t& cursor);
    void addHandler(ident_t ident, handler_t handler);
    void addHandlerWithArg(ident_t ident, handler_with_arg_t handler);

private:
    std::unordered_map<char, handler_t> m_Handlers;
    std::unordered_map<char, handler_with_arg_t> m_HandlersWithArg;
};
