#pragma once

#include <atomic>
#include <cstdint>
#include <unistd.h>
#include <vector>

struct TerminalData {};

class Terminal {
public:
    void open();
    void close();
    bool shouldClose();
    std::vector<uint8_t> read();
    void write(uint8_t buf[], size_t len);

private:
    // These are set once and not changed later, so they don't need to be atomic
    int m_MasterFd;
    int m_SlaveFd;
    pid_t m_TermProcessPid;

    std::atomic<bool> m_ShouldClose = false;
};
