#pragma once

#include <atomic>
#include <cstdint>
#include <cstring>
#include <exception>
#include <unistd.h>
#include <vector>

using TerminalRaw = std::vector<uint8_t>;

class Terminal {
public:
    void open(int windowWidth, int windowHeight);
    void close();
    bool shouldClose();
    TerminalRaw read();
    void write(uint8_t buf[], size_t len);

private:
    // These are set on open() and not changed later, so they don't need to be atomic
    int m_MasterFd;
    int m_SlaveFd;
    pid_t m_TermProcessPid;

    std::atomic<bool> m_ShouldClose = false;
};

class TerminalReadException : std::exception {
public:
    virtual const char* what() const noexcept override {
        return std::strerror(errno);
    }
};
