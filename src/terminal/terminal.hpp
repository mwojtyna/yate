#pragma once

#include "terminal_buffer.hpp"
#include "types.hpp"
#include <atomic>
#include <cstdint>
#include <cstring>
#include <exception>
#include <functional>
#include <shared_mutex>
#include <string>
#include <vector>

class Terminal {
public:
    void open(int windowWidth, int windowHeight);
    void close();
    bool shouldClose() const;
    std::vector<uint8_t> read() const;
    void write(std::vector<uint8_t>&& bytes);

    void getBuf(std::function<void(const TerminalBuf&)> cb) const;
    void getBufMut(std::function<void(TerminalBuf&)> cb);

    cursor_t getCursor() const;
    void getCursorMut(std::function<void(cursor_t&)> cb);

private:
    // These are set on open() and not changed later, so they don't need to be thread-safe
    int m_MasterFd;
    int m_SlaveFd;
    pid_t m_TermProcessPid;
    std::string m_PtyPath;

    std::atomic<bool> m_ShouldClose;
    TerminalBuf m_Buf;
    mutable std::shared_mutex m_BufMutex;
    cursor_t m_Cursor;
    mutable std::shared_mutex m_CursorMutex;
};

class TerminalReadException : std::exception {
public:
    virtual const char* what() const noexcept override {
        return std::strerror(errno);
    }
};
