#pragma once

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
    void parse(std::vector<uint8_t>::iterator& it,
               std::vector<uint8_t>::iterator end);

    void addHandlerString(ident_t ident,
                          handlerfn_t<std::vector<std::string>> handler);
    void addHandlerByte(ident_t ident,
                        handlerfn_t<std::vector<std::vector<uint8_t>>> handler);

    static std::vector<std::string>
    parseArgs(std::vector<uint8_t>::iterator& it,
              std::vector<uint8_t>::iterator end);

private:
    std::unordered_map<ident_t, handlerfn_t<std::vector<std::string>>>
        m_StringHandlers;
    std::unordered_map<ident_t, handlerfn_t<std::vector<std::vector<uint8_t>>>>
        m_ByteHandlers;
};
