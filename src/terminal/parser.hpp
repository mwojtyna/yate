#pragma once

#include "csi_parser.hpp"
#include "osc_parser.hpp"
#include "unicode.hpp"
#include <glm/ext/vector_float4.hpp>
#include <unordered_set>
#include <vector>

class Parser {
public:
    Parser(CsiParser&& csiParser, OscParser&& oscParser);

    /// Returns: list of parsed character codepoints
    std::unordered_set<codepoint_t>
    parseAndModifyTermBuf(std::vector<uint8_t>& data);

    static std::vector<uint32_t> parsePs(iter_t& it, iter_t end);
    static bool isEol(codepoint_t character);

private:
    ParserState m_State;
    CsiParser m_CsiParser;
    OscParser m_OscParser;
};
