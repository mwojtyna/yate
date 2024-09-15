#pragma once

#include "csi_parser.hpp"
#include "osc_parser.hpp"
#include <glm/ext/vector_float4.hpp>
#include <vector>

class Parser {
public:
    Parser(CsiParser& csiParser, OscParser& oscParser);
    std::vector<Cell> parse(termbuf_t& data);

    static std::optional<uint32_t> parsePs(iter_t& it, iter_t end);
    static void skipSpaces(iter_t& it, iter_t end);

private:
    CsiParser m_CsiParser;
    OscParser m_OscParser;
};
