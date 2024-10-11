#ifdef __APPLE__
#include <crt_externs.h>
#include <util.h>
#define environ (*_NSGetEnviron())
#else
#include <pty.h>
#endif

#include "../utils.hpp"
#include "terminal.hpp"
#include <atomic>
#include <cerrno>
#include <csignal>
#include <cstdlib>
#include <cstring>
#include <fcntl.h>
#include <pwd.h>
#include <spdlog/spdlog.h>
#include <sys/ioctl.h>
#include <unistd.h>

#define FATAL_CHILD(...)                                                       \
    SPDLOG_ERROR(__VA_ARGS__);                                                 \
    kill(getppid(), SIGTERM);

Terminal::TerminalData* Terminal::s_Data;

// TODO: Divide window into terminal rows and columns
void Terminal::open(int windowWidth, int windowHeight) {
    s_Data = new TerminalData();

    if (openpty(&s_Data->masterFd, &s_Data->slaveFd, nullptr, nullptr,
                nullptr)) {
        FATAL("Failed to open pty: {}", strerror(errno));
    }

    if (fcntl(s_Data->masterFd, F_SETFD, FD_CLOEXEC) == -1) {
        FATAL("Failed setting CLOEXEC for master pty");
    }
    if (fcntl(s_Data->slaveFd, F_SETFD, FD_CLOEXEC) == -1) {
        FATAL("Failed setting CLOEXEC for slave pty");
    }

    pid_t pid = fork();
    if (pid == 0) {
        // Child process
        if (dup2(s_Data->slaveFd, STDIN_FILENO) == -1) {
            FATAL_CHILD("Failed piping stdin: {}", std::strerror(errno));
        }
        if (dup2(s_Data->slaveFd, STDOUT_FILENO) == -1) {
            FATAL_CHILD("Failed piping stdout: {}", std::strerror(errno));
        }
        if (dup2(s_Data->slaveFd, STDERR_FILENO) == -1) {
            FATAL_CHILD("Failed piping stderr: {}", std::strerror(errno));
        }

        const char* home = getenv("HOME");
        struct passwd* user = getpwuid(getuid());
        if (home == nullptr || strcmp(home, "") == 0) {
            if (user == nullptr) {
                FATAL_CHILD(
                    "Failed getting home directory of user: getenv() and "
                    "getpwuid() both failed");
            }
            home = user->pw_dir;
        }
        if (chdir(home) == -1) {
            FATAL_CHILD("Failed setting cwd '{}': {}", home,
                        std::strerror(errno));
        }

        if (setsid() == -1) {
            FATAL_CHILD("Failed creating session: {}", std::strerror(errno));
        }

        if (ioctl(s_Data->slaveFd, TIOCSCTTY, 0) == -1) {
            FATAL_CHILD("Failed setting controlling terminal: {}",
                        std::strerror(errno));
        }

        const char* argv[] = {"bash", nullptr};
        std::vector<const char*> envs;
        for (size_t i = 0; environ[i] != nullptr; i++) {
            envs.push_back(environ[i]);
        }
        envs.push_back("SHELL=/bin/bash");
        envs.push_back(nullptr);
        if (execve("/bin/bash", (char* const*)argv,
                   (char* const*)envs.data()) == -1) {
            FATAL_CHILD("Failed opening shell: {}", std::strerror(errno));
        }
    } else if (pid != -1) {
        // Parent process
        s_Data->termProcessPid = pid;
    } else {
        FATAL("Failed forking process: {}", strerror(errno));
    }

    SPDLOG_DEBUG("Opened pty");
}

void Terminal::close() {
    s_Data->shouldClose.store(true, std::memory_order_relaxed);
    ::close(s_Data->masterFd);
    ::close(s_Data->slaveFd);
    kill(s_Data->termProcessPid, SIGKILL);

    delete s_Data;
    SPDLOG_DEBUG("Closed pty");
}

std::vector<uint8_t> Terminal::read() {
    constexpr size_t BUF_SIZE = 65535;

    std::vector<uint8_t> buf(BUF_SIZE);
    int bytesRead = ::read(s_Data->masterFd, buf.data(), BUF_SIZE);

    if (bytesRead >= 0) {
        buf.resize(bytesRead);
    } else {
        throw TerminalReadException();
    }

    return buf;
}

void Terminal::write(std::vector<codepoint_t>&& codepoints) {
    ::write(s_Data->masterFd, codepoints.data(), codepoints.size());
    SPDLOG_TRACE("Written to terminal:");
    hexdump<codepoint_t>(codepoints.data(), codepoints.size(),
                         SPDLOG_LEVEL_TRACE);
}

bool Terminal::shouldClose() {
    return s_Data->shouldClose.load(std::memory_order_relaxed);
}

void Terminal::getBuf(std::function<void(const TerminalBuf&)> cb) {
    std::shared_lock lock(s_Data->bufMutex);
    cb(s_Data->buf);
}

void Terminal::getBufMut(std::function<void(TerminalBuf&)> cb) {
    std::unique_lock lock(s_Data->bufMutex);
    cb(s_Data->buf);
}

void Terminal::getCursor(std::function<void(const glm::vec2&)> cb) {
    std::shared_lock lock(s_Data->cursorMutex);
    cb(s_Data->cursor);
}

glm::vec2 Terminal::getCursor() {
    std::shared_lock lock(s_Data->cursorMutex);
    return s_Data->cursor;
}

void Terminal::getCursorMut(std::function<void(glm::vec2&)> cb) {
    std::unique_lock lock(s_Data->cursorMutex);
    cb(s_Data->cursor);
}

void Terminal::setCursor(glm::vec2 value) {
    std::unique_lock lock(s_Data->cursorMutex);
    s_Data->cursor = value;
}
