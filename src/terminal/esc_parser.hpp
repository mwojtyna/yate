#pragma once

#include "types.hpp"
#include <functional>
#include <optional>
#include <string>
#include <unordered_map>

class EscParser {
    using ident_t = char;
    using handler_t =
        std::function<void(ParserState&, std::optional<std::string>)>;

public:
    void parse(iter_t& it, iter_t end, ParserState& parserState);
    void addHandler(char ident, handler_t handler, bool acceptsArg = false);

private:
    struct Handler {
        handler_t function;
        bool acceptsArg;
    };

    std::unordered_map<char, Handler> m_Handlers;
};
