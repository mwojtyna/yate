#include "csi_parser.hpp"
#include "parser.hpp"
#include "types.hpp"
#include <optional>
#include <spdlog/common.h>
#include <spdlog/fmt/bin_to_hex.h>
#include <spdlog/spdlog.h>

bool CsiIdent::operator==(const CsiIdent& rhs) const {
    bool equals = true;

    equals &= prefix == rhs.prefix;
    equals &= intermediate == rhs.intermediate;
    equals &= final == rhs.final;

    return equals;
}

void CsiParser::parse(iter_t& it, iter_t end) {
    std::optional<char> prefix = std::nullopt;
    if (!std::isdigit(*it) && !std::isalpha(*it)) {
        prefix.emplace(*it);
        it++;
    }
    SPDLOG_TRACE("CSI:prefix = {}", optionalToString(prefix));

    std::optional<uint32_t> ps = Parser::parsePs(it, end);
    SPDLOG_TRACE("CSI:Ps = {}", optionalToString(ps));

    std::optional<char> intermediate = std::nullopt;
    // If is a symbol
    if (!std::isdigit(*it) && !std::isalpha(*it)) {
        intermediate.emplace(*it);
        it++;
    }
    SPDLOG_TRACE("CSI:intermediate = {}", optionalToString(intermediate));

    char final = *it;
    SPDLOG_TRACE("CSI:final = {}", final);

    CsiIdent ident = CsiIdent{
        .prefix = prefix,
        .intermediate = intermediate,
        .final = final,
    };

    if (m_Handlers.contains(ident)) {
        if (ps.has_value()) {
            m_Handlers[ident](std::vector{ps.value()});
        } else {
            m_Handlers[ident](std::vector<uint32_t>());
        }
    } else {
        SPDLOG_WARN("Unsupported CSI sequence with: prefix={}, Ps={}, "
                    "intermediate={}, final={}",
                    optionalToString(prefix), optionalToString(ps),
                    optionalToString(intermediate), final);
    }
}

void CsiParser::addHandler(CsiIdent& ident, handlerfn_t handler) {
    m_Handlers[ident] = handler;
}
