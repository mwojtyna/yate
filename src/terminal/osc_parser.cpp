#include "osc_parser.hpp"
#include "codes.hpp"
#include "parser.hpp"
#include "types.hpp"
#include <spdlog/fmt/bin_to_hex.h>
#include <spdlog/spdlog.h>
#include <string>

void OscParser::parse(iter_t& it, iter_t end) {
    const std::optional<uint8_t> ident = Parser::parsePs(it, end);
    if (!ident.has_value()) {
        SPDLOG_ERROR("OSC:ident - cannot be optional:{}",
                     spdlog::to_hex(it, end));
        return;
    }
    SPDLOG_TRACE("OSC:ident = {}", ident.value());

    if (*it != ARG_SEPARATOR) {
        SPDLOG_ERROR("OSC - expected separator:{}", spdlog::to_hex(it, end));
        return;
    }
    it++;

    uint8_t identv = ident.value();
    std::vector<std::string> args = parseArgs(it, end);
    if (m_Handlers.contains(identv)) {
        m_Handlers[identv](std::move(args));
    } else {
        SPDLOG_WARN("Unsupported OSC sequence with ident={}", identv);
    }
}

void OscParser::addHandler(ident_t ident, handlerfn_t handler) {
    m_Handlers[ident] = handler;
}

// STATIC
std::vector<std::string> OscParser::parseArgs(iter_t& it, iter_t end) {
    std::vector<std::string> args{""};

    size_t argIdx = 0;
    for (; it < end; it++) {
        if (*it == c0::BEL || *it == c1::ST) {
            break;
        }
        if (*it == ARG_SEPARATOR) {
            argIdx++;
            args.push_back("");
            continue;
        }

        args[argIdx] += *it;
    }

    return args;
}
