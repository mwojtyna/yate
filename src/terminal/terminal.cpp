#ifdef __APPLE__
#include <crt_externs.h>
#include <util.h>
#define environ (*_NSGetEnviron())
#else
#include <pty.h>
#endif

#include "../utils.hpp"
#include "terminal.hpp"
#include "types.hpp"
#include <atomic>
#include <cerrno>
#include <csignal>
#include <cstdlib>
#include <cstring>
#include <fcntl.h>
#include <filesystem>
#include <format>
#include <pwd.h>
#include <spdlog/spdlog.h>
#include <string>
#include <sys/ioctl.h>
#include <unistd.h>

#define FATAL_CHILD(...)                                                       \
    SPDLOG_ERROR(__VA_ARGS__);                                                 \
    kill(getppid(), SIGTERM);

void Terminal::open(int windowWidth, int windowHeight) {

    // TODO: Size based on window
    struct winsize winsize = {
        .ws_row = 41, .ws_col = 120, .ws_xpixel = 0, .ws_ypixel = 0};
    if (openpty(&m_MasterFd, &m_SlaveFd, nullptr, nullptr, &winsize)) {
        FATAL("Failed to open pty: {}", strerror(errno));
    }

    m_PtyPath.reserve(256);
    ptsname_r(m_MasterFd, const_cast<char*>(m_PtyPath.c_str()), 256);

    if (fcntl(m_MasterFd, F_SETFD, FD_CLOEXEC) == -1) {
        FATAL("Failed setting CLOEXEC for master pty");
    }
    if (fcntl(m_SlaveFd, F_SETFD, FD_CLOEXEC) == -1) {
        FATAL("Failed setting CLOEXEC for slave pty");
    }

    pid_t pid = fork();
    if (pid == 0) {
        // Child process
        if (dup2(m_SlaveFd, STDIN_FILENO) == -1) {
            FATAL_CHILD("Failed piping stdin: {}", std::strerror(errno));
        }
        if (dup2(m_SlaveFd, STDOUT_FILENO) == -1) {
            FATAL_CHILD("Failed piping stdout: {}", std::strerror(errno));
        }
        if (dup2(m_SlaveFd, STDERR_FILENO) == -1) {
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

        if (ioctl(m_SlaveFd, TIOCSCTTY, 0) == -1) {
            FATAL_CHILD("Failed setting controlling terminal: {}",
                        std::strerror(errno));
        }

        std::vector<const char*> envs;
        for (size_t i = 0; environ[i] != nullptr; i++) {
            envs.push_back(environ[i]);
        }
        envs.push_back(std::format("SHELL={}", user->pw_shell).c_str());
        envs.push_back(std::format("TTY={}", m_PtyPath.c_str()).c_str());
        envs.push_back(nullptr);

        const char* argv[] = {
            std::filesystem::path(user->pw_shell).filename().c_str(), nullptr};
        if (execve(user->pw_shell, const_cast<char* const*>(argv),
                   const_cast<char* const*>(envs.data())) == -1) {
            FATAL_CHILD("Failed opening shell: {}", std::strerror(errno));
        }
    } else if (pid != -1) {
        // Parent process
        m_TermProcessPid = pid;
    } else {
        FATAL("Failed forking process: {}", strerror(errno));
    }

    SPDLOG_DEBUG("Opened pty");
}

void Terminal::close() {
    m_ShouldClose.store(true, std::memory_order_relaxed);
    ::close(m_MasterFd);
    ::close(m_SlaveFd);
    kill(m_TermProcessPid, SIGKILL);
    SPDLOG_DEBUG("Closed pty");
}

std::vector<uint8_t> Terminal::read() const {
    constexpr size_t CHUNK_SIZE = 1024;
    std::vector<uint8_t> buffer;
    std::vector<uint8_t> tempBuf(CHUNK_SIZE);

    while (true) {
        int bytesRead = ::read(m_MasterFd, tempBuf.data(), CHUNK_SIZE);

        if (bytesRead > 0) {
            // TODO: Parse escape codes and determine if an escape sequence hasn't ended yet,
            // meaning we have to wait until the next chunk
            // (sometimes we read too quickly and a part of an escape sequence is cut off)
            // Do the same for UTF-8
            buffer.insert(buffer.end(), tempBuf.begin(),
                          tempBuf.begin() + bytesRead);
            if (bytesRead < CHUNK_SIZE) {
                // Less than BUF_SIZE bytes indicates we've reached the end of current data.
                break;
            }
        } else if (bytesRead == 0) {
            // EOF reached
            break;
        } else {
            // Read error/user closed terminal
            throw TerminalReadException();
        }
    }

    return buffer;
}

void Terminal::write(std::vector<uint8_t>&& bytes) {
    ::write(m_MasterFd, bytes.data(), bytes.size());
    SPDLOG_TRACE("Written to terminal:");
    hexdump(bytes.data(), bytes.size(), SPDLOG_LEVEL_TRACE);
}

bool Terminal::shouldClose() const {
    return m_ShouldClose.load(std::memory_order_relaxed);
}

void Terminal::getBuf(std::function<void(const TerminalBuf&)> cb) const {
    std::shared_lock lock(m_BufMutex);
    cb(m_Buf);
}

void Terminal::getBufMut(std::function<void(TerminalBuf&)> cb) {
    std::unique_lock lock(m_BufMutex);
    cb(m_Buf);
}

cursor_t Terminal::getCursor() const {
    std::shared_lock lock(m_CursorMutex);
    return m_Cursor;
}

void Terminal::getCursorMut(std::function<void(cursor_t&)> cb) {
    std::unique_lock lock(m_CursorMutex);
    cb(m_Cursor);
}
