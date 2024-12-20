#pragma once

#include "../utils.hpp"
#include "codes.hpp"
#include "terminal_buffer.hpp"
#include "types.hpp"
#include <cstdint>
#include <functional>
#include <glm/ext/vector_float2.hpp>
#include <optional>

struct CsiIdent {
    std::optional<char> prefix;
    std::optional<char> intermediate;
    char final;

    bool operator==(const CsiIdent& rhs) const;
    constexpr std::vector<uint8_t> data(std::optional<uint32_t> ps) const {
        std::vector<uint8_t> out{c0::ESC, c0::CSI};

        if (prefix.has_value()) {
            out.push_back(prefix.value());
        }
        if (ps.has_value()) {
            out.push_back(ps.value());
        }
        if (intermediate.has_value()) {
            out.push_back(intermediate.value());
        }
        out.push_back(final);

        return out;
    }
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
    using handlerfn_t = std::function<void(
        const std::vector<uint32_t>, ParserState&, TerminalBuf&, cursor_t&)>;

public:
    void parse(iter_t& it, iter_t end, ParserState& parserState,
               TerminalBuf& termBuf, cursor_t& cursor);
    void addHandler(const CsiIdent& ident, handlerfn_t handler);

private:
    std::unordered_map<CsiIdent, handlerfn_t> m_Handlers;
};
