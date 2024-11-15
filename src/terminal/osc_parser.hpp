#pragma once

#include "terminal_buffer.hpp"
#include "types.hpp"
#include <cstdint>
#include <functional>
#include <string>
#include <unordered_map>
#include <vector>

class OscParser {
    using ident_t = uint32_t;
    using handlerfn_t = std::function<void(const std::vector<std::string>,
                                           TerminalBuf&, cursor_t&)>;

public:
    void parse(iter_t& it, iter_t end, TerminalBuf& termBuf, cursor_t& cursor);
    void addHandler(ident_t ident, handlerfn_t handler);

    static std::vector<std::string> parseArgs(iter_t& it, iter_t end);

private:
    std::unordered_map<ident_t, handlerfn_t> m_Handlers;
};
