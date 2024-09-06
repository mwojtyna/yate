#pragma once

#include "types.hpp"
#include <cstdint>
#include <functional>
#include <string>
#include <unordered_map>
#include <vector>

class OscParser {
    using ident_t = uint8_t;
    template <typename T>
    using handlerfn_t = std::function<void(T)>;

public:
    /// OSC commands don't return any data
    void parse(iter_t& it, iter_t end);

    void addStringHandler(ident_t ident,
                          handlerfn_t<std::vector<std::string>> handler);
    void addBytesHandler(ident_t ident,
                         handlerfn_t<std::vector<termbuf_t>> handler);

    static std::vector<std::string> parseArgs(iter_t& it, iter_t end);

private:
    std::unordered_map<ident_t, handlerfn_t<std::vector<std::string>>>
        m_StringHandlers;
    std::unordered_map<ident_t, handlerfn_t<std::vector<termbuf_t>>>
        m_ByteHandlers;
};
