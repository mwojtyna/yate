#include "esc_parser.hpp"
#include "parser.hpp"
#include <spdlog/spdlog.h>
#include <string>

void EscParser::parse(iter_t& it, iter_t end, ParserState& parserState,
                      TerminalBuf& termBuf, cursor_t& cursor) {
    char ident = *it;
    if (!m_Handlers.contains(ident) && !m_HandlersWithArg.contains(ident)) {
        SPDLOG_WARN("Unsupported escape sequence 'ESC {}' ({:#x})", (char)ident,
                    (uint8_t)ident);
        return;
    }

    if (m_Handlers.contains(ident)) {
        m_Handlers[ident](parserState, termBuf, cursor);
    } else {
        // Must be a handler with arg
        std::string arg = Parser::readUntilST(++it, end);
        m_HandlersWithArg[ident](parserState, termBuf, cursor, arg);
    }
}

void EscParser::addHandler(ident_t ident, handler_t handler) {
    m_Handlers[ident] = handler;
}

void EscParser::addHandlerWithArg(ident_t ident, handler_with_arg_t handler) {
    m_HandlersWithArg[ident] = handler;
}
