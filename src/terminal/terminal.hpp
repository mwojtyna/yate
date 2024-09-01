#pragma once

#include <cstdint>
#include <unistd.h>
#include <vector>

struct TerminalData {
    int masterFd;
    int slaveFd;
    pid_t termProcessPid;
};

class Terminal {
public:
    Terminal() = delete;
    Terminal(Terminal& terminal) = delete;
    Terminal(Terminal&& terminal) = delete;

    static void open();
    static void close();
    static std::vector<uint8_t> read();
    static void write();

private:
    static TerminalData s_Data;
};
