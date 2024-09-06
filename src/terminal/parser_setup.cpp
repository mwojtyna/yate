#include "osc_parser.hpp"
#include "parser.hpp"
#include <spdlog/spdlog.h>

void setWindowTitle(std::vector<std::string> data) {
    SPDLOG_DEBUG("Set window title: '{}'", data[0]);
}

Parser parser_setup() {
    OscParser osc;
    osc.addStringHandler(0, &setWindowTitle);

    Parser parser(osc);

    return parser;
}
