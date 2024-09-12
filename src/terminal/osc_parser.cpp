#include "osc_parser.hpp"
#include "constants.hpp"
#include "parser.hpp"
#include "types.hpp"
#include <spdlog/fmt/bin_to_hex.h>
#include <spdlog/spdlog.h>
#include <string>

void OscParser::addStringHandler(
    ident_t ident, handlerfn_t<std::vector<std::string>> handler) {
    m_StringHandlers[ident] = handler;
}

void OscParser::addBytesHandler(ident_t ident,
                                handlerfn_t<std::vector<termbuf_t>> handler) {
    m_ByteHandlers[ident] = handler;
}

void OscParser::parse(iter_t& it, iter_t end) {
    const std::optional<uint8_t> ident = Parser::parsePs(it, end);
    if (!ident.has_value()) {
        SPDLOG_ERROR("OSC:Ps - cannot be optional:{}", spdlog::to_hex(it, end));
        return;
    }

    if (*it != SEPARATOR) {
        SPDLOG_ERROR("OSC - expected separator:{}", spdlog::to_hex(it, end));
        return;
    }
    it++;

    uint8_t identv = ident.value();
    switch (identv) {
    case 0: {
        std::vector<std::string> args = parseArgs(it, end);
        m_StringHandlers[identv](args);
        break;
    }
    }
}

// STATIC
std::vector<std::string> OscParser::parseArgs(iter_t& it, iter_t end) {
    std::vector<std::string> args;
    args.push_back("");

    size_t argIdx = 0;
    for (; it < end; it++) {
        if (*it == BELL || *it == ST) {
            break;
        }
        if (*it == SEPARATOR) {
            argIdx++;
            args.push_back("");
            continue;
        }

        args[argIdx] += *it;
    }

    return args;
}
