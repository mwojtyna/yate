#include "../terminal/codes.hpp"
#include "../terminal/parser.hpp"
#include <catch2/catch_test_macros.hpp>
#include <string>
#include <vector>

TEST_CASE("OSC 0 - set window title") {
    bool wasCalled = false;
    CsiParser csi;

    OscParser osc;
    osc.addHandler(0, [&wasCalled](std::vector<std::string> args) {
        wasCalled = true;
        REQUIRE(args.size() == 1);
        REQUIRE(args[0] == "title");
    });

    std::vector<uint8_t> data = {c0::ESC, c0::OSC, '0', ';', 't',
                                 'i',     't',     'l', 'e', c0::BEL};
    Parser parser(std::move(csi), std::move(osc));
    parser.parse(data);

    REQUIRE(wasCalled);
}

TEST_CASE("OSC 2 - set window title") {
    bool wasCalled = false;
    CsiParser csi;

    OscParser osc;
    osc.addHandler(2, [&wasCalled](std::vector<std::string> args) {
        wasCalled = true;
        REQUIRE(args.size() == 1);
        REQUIRE(args[0] == "title");
    });

    std::vector<uint8_t> data = {c0::ESC, c0::OSC, '2', ';', 't',
                                 'i',     't',     'l', 'e', c0::BEL};
    Parser parser(std::move(csi), std::move(osc));
    parser.parse(data);

    REQUIRE(wasCalled);
}
