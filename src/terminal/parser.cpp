#include "parser.hpp"
#include "glm/ext/vector_float4.hpp"
#include "terminal.hpp"

static std::vector<Codepoint> testText = {
    62211, 32,  57520, 32,    61564, 32,    126, 47,    100, 101, 118, 101, 108,
    111,   112, 101,   114,   47,    121,   97,  116,   101, 47,  98,  117, 105,
    108,   100, 32,    57520, 32,    61715, 32,  61734, 32,  109, 97,  105, 110,
    32,    33,  49,    32,    57520, 10,    90,  97,    380, 243, 322, 263, 32,
    103,   281, 347,   108,   261,   32,    106, 97,    378, 324, 33,  10,  84,
    104,   101, 32,    113,   117,   105,   99,  107,   32,  98,  114, 111, 119,
    110,   32,  102,   111,   120,   32,    106, 117,   109, 112, 115, 32,  111,
    118,   101, 114,   32,    116,   104,   101, 32,    108, 97,  122, 121, 32,
    100,   111, 103,   46,    10,    9,     113, 33,    64,  35,  36,  37,  94,
    38,    42,  40,    41,    45,    95,    61,  43,    91,  123, 93,  125, 59,
    58,    39,  34,    44,    60,    46,    62,  47,    63};

std::vector<ParsedChunk> TerminalParser::parse(TerminalRaw& raw) {
    std::vector<ParsedChunk> chunks;
    chunks.push_back(ParsedChunk{
        .bgColor = glm::vec4(0),
        .fgColor = glm::vec4(1),
        .text = testText,
    });
    return chunks;
}
