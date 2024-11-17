#pragma once

#include "csi_parser.hpp"
#include "esc_parser.hpp"
#include "osc_parser.hpp"
#include "terminal_buffer.hpp"
#include "types.hpp"
#include "unicode.hpp"
#include <SDL.h>
#include <glm/ext/vector_float4.hpp>
#include <unordered_set>
#include <vector>

class Parser {
public:
    Parser(CsiParser&& csiParser, OscParser&& oscParser, EscParser&& escParser);

    /// Returns: list of parsed character codepoints
    std::unordered_set<codepoint_t>
    parse(std::vector<uint8_t>& data, TerminalBuf& termBuf, cursor_t& cursor);

    static std::vector<uint32_t> parsePs(iter_t& it, iter_t end);
    static std::string readUntilST(iter_t& it, iter_t end);
    static bool isEol(codepoint_t character);

private:
    ParserState m_State;
    CsiParser m_CsiParser;
    OscParser m_OscParser;
    EscParser m_EscParser;
};
