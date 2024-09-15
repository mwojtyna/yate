#pragma once

#include "../utils.hpp"
#include "types.hpp"
#include <cstdint>
#include <functional>
#include <optional>

struct CsiIdent {
    std::optional<char> prefix;
    std::optional<char> intermediate;
    char final;

    bool operator==(const CsiIdent& rhs) const;
};

template <>
struct std::hash<CsiIdent> {
    std::size_t operator()(const CsiIdent& v) const {
        size_t res = 0;
        hashCombine(res, v.prefix);
        hashCombine(res, v.intermediate);
        hashCombine(res, v.final);
        return res;
    }
};

class CsiParser {
    using handlerfn_t = std::function<void(std::vector<uint32_t>)>;

public:
    void parse(iter_t& it, iter_t end);

private:
    std::unordered_map<CsiIdent, handlerfn_t> m_Handlers;
};
