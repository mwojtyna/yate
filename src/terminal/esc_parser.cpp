#include "esc_parser.hpp"
#include "parser.hpp"
#include "spdlog/spdlog.h"
#include <optional>
#include <string>

void EscParser::parse(iter_t& it, iter_t end, ParserState& parserState,
                      TerminalBuf& termBuf, cursor_t& cursor) {
    char character = *it;
    if (!m_Handlers.contains(character)) {
        SPDLOG_WARN("Unsupported escape sequence 'ESC {}' ({:#x})",
                    (char)character, (uint8_t)character);
        return;
    }

    if (m_Handlers[character].acceptsArg) {
        std::string arg = Parser::readUntilST(it, end);
        m_Handlers[character].function(parserState, termBuf, cursor,
                                       std::move(arg));
    } else {
        m_Handlers[character].function(parserState, termBuf, cursor,
                                       std::nullopt);
    }
}

void EscParser::addHandler(ident_t ident, handler_t handler, bool acceptsArg) {
    m_Handlers[ident] = Handler{.function = handler, .acceptsArg = acceptsArg};
}
