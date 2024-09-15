#pragma once

#include "../rendering/font.hpp"
#include "types.hpp"
#include <atomic>
#include <cstring>
#include <exception>
#include <unistd.h>

class Terminal {
public:
    Terminal() = delete;
    Terminal(Terminal& terminal) = delete;
    Terminal(Terminal&& terminal) = delete;
    ~Terminal() = delete;

    static void open(int windowWidth, int windowHeight);
    static void close();
    static bool shouldClose();
    static std::vector<uint8_t> read();
    static void write(codepoint_t codepoint);

private:
    // These are set on open() and not changed later, so they don't need to be atomic
    static int s_MasterFd;
    static int s_SlaveFd;
    static pid_t s_TermProcessPid;
    static std::atomic<bool> s_ShouldClose;
};

class TerminalReadException : std::exception {
public:
    virtual const char* what() const noexcept override {
        return std::strerror(errno);
    }
};
