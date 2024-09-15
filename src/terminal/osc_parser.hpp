#pragma once

#include "types.hpp"
#include <cstdint>
#include <functional>
#include <string>
#include <unordered_map>
#include <vector>

class OscParser {
    using ident_t = uint8_t;
    using handlerfn_t = std::function<void(const std::vector<std::string>&&)>;

public:
    void parse(iter_t& it, iter_t end);
    void addHandler(ident_t ident, handlerfn_t handler);

    static std::vector<std::string> parseArgs(iter_t& it, iter_t end);

private:
    std::unordered_map<ident_t, handlerfn_t> m_Handlers;
};
