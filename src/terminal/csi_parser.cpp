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
        prefix = *it;
        it++;
    }
    SPDLOG_TRACE("CSI:prefix = {}", optionalToString(prefix));

    const std::vector<uint32_t> ps = Parser::parsePs(it, end);
    SPDLOG_TRACE("CSI:Ps = {}", vectorToString(ps));

    std::optional<char> intermediate = std::nullopt;
    if (!std::isdigit(*it) && !std::isalpha(*it)) {
        intermediate = *it;
        it++;
    }
    SPDLOG_TRACE("CSI:intermediate = {}", optionalToString(intermediate));

    const char final = *it;
    SPDLOG_TRACE("CSI:final = {}", final);

    const CsiIdent ident = CsiIdent{
        .prefix = prefix,
        .intermediate = intermediate,
        .final = final,
    };

    if (m_Handlers.contains(ident)) {
        m_Handlers[ident](!ps.empty() ? ps : std::vector<uint32_t>());
    } else {
        SPDLOG_WARN("Unsupported CSI sequence with: prefix={}, Ps={}, "
                    "intermediate={}, final={}",
                    optionalToString(prefix), vectorToString(ps),
                    optionalToString(intermediate), final);
    }
}

void CsiParser::addHandler(const CsiIdent& ident, handlerfn_t handler) {
    m_Handlers[ident] = handler;
}
