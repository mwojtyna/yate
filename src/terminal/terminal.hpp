#pragma once

#include "terminal_buffer.hpp"
#include <atomic>
#include <cstdint>
#include <cstring>
#include <exception>
#include <functional>
#include <glm/ext/vector_float2.hpp>
#include <shared_mutex>
#include <string>
#include <vector>

class Terminal {
public:
    struct TerminalData {
        // These are set on open() and not changed later, so they don't need to be atomic
        int masterFd;
        int slaveFd;
        pid_t termProcessPid;
        std::string ptyPath;

        std::atomic<bool> shouldClose;
        TerminalBuf buf;
        std::shared_mutex bufMutex;
        glm::vec2 cursor;
        std::shared_mutex cursorMutex;
    };

    Terminal() = delete;
    Terminal(Terminal& terminal) = delete;
    Terminal(Terminal&& terminal) = delete;
    ~Terminal() = delete;

    static void open(int windowWidth, int windowHeight);
    static void close();
    static bool shouldClose();
    static std::vector<uint8_t> read();
    static void write(std::vector<uint8_t>&& bytes);

    static void getBuf(std::function<void(const TerminalBuf&)> cb);
    static void getBufMut(std::function<void(TerminalBuf&)> cb);

    static void getCursor(std::function<void(const glm::vec2&)> cb);
    static glm::vec2 getCursor();
    static void getCursorMut(std::function<void(glm::vec2&)> cb);
    static void setCursor(glm::vec2 value);

private:
    static TerminalData* s_Data;
};

class TerminalReadException : std::exception {
public:
    virtual const char* what() const noexcept override {
        return std::strerror(errno);
    }
};
