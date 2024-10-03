#pragma once

#include "../rendering/types.hpp"
#include "terminal_buffer.hpp"
#include <atomic>
#include <cstring>
#include <exception>
#include <functional>
#include <shared_mutex>
#include <unistd.h>
#include <vector>

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

    static void getBuf(std::function<void(const TerminalBuf&)> cb);
    static void getBufMut(std::function<void(TerminalBuf&)> cb);

private:
    // These are set on open() and not changed later, so they don't need to be atomic
    static int s_MasterFd;
    static int s_SlaveFd;
    static pid_t s_TermProcessPid;

    static std::atomic<bool> s_ShouldClose;
    static TerminalBuf s_Buf;
    static std::shared_mutex s_BufMutex;
};

class TerminalReadException : std::exception {
public:
    virtual const char* what() const noexcept override {
        return std::strerror(errno);
    }
};
