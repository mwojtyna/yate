#include "osc_parser.hpp"
#include "../utils.hpp"
#include "codes.hpp"
#include "types.hpp"
#include <spdlog/fmt/bin_to_hex.h>
#include <spdlog/spdlog.h>
#include <string>

void OscParser::parse(iter_t& it, iter_t end, TerminalBuf& termBuf,
                      cursor_t& cursor) {
    std::optional<uint32_t> ident = std::nullopt;
    std::string identStr = "";
    for (; std::isdigit(*it); it++) {
        identStr += *it;
    }
    ident = std::stoi(identStr);

    if (!ident.has_value()) {
        SPDLOG_ERROR("OSC:ident - cannot be optional:");
        std::vector<uint8_t> osc(it, end);
        hexdump(osc.data(), osc.size(), SPDLOG_LEVEL_ERROR);
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
        m_Handlers[identv](args, termBuf, cursor);
    } else {
        SPDLOG_WARN("Unsupported OSC sequence with ident={}", identv);
    }
}

void OscParser::addHandler(ident_t ident, handlerfn_t handler) {
    m_Handlers[ident] = handler;
}

// STATIC
std::vector<std::string> OscParser::parseArgs(iter_t& it, iter_t end) {
    std::vector<std::string> args;

    size_t argIdx = 0;
    for (; it < end; it++) {
        // Finish sequence
        if (*it == c0::BEL || *it == c1::ST) {
            break;
        }
        if (*it == c0::ESC && *(it + 1) == c0::ST) {
            it++;
            break;
        }

        if (args.empty()) {
            args.push_back("");
        }

        // Next argument
        if (*it == ARG_SEPARATOR) {
            argIdx++;
            args.push_back("");
            continue;
        }

        args[argIdx] += *it;
    }

    return args;
}
