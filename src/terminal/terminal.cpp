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

int Terminal::s_MasterFd;
int Terminal::s_SlaveFd;
pid_t Terminal::s_TermProcessPid;
std::atomic<bool> Terminal::s_ShouldClose;
TerminalBuf Terminal::s_Buf;
std::shared_mutex Terminal::s_BufMutex;

// TODO: Divide window into terminal rows and columns
void Terminal::open(int windowWidth, int windowHeight) {
    if (openpty(&s_MasterFd, &s_SlaveFd, nullptr, nullptr, nullptr)) {
        FATAL("Failed to open pty: {}", strerror(errno));
    }

    if (fcntl(s_MasterFd, F_SETFD, FD_CLOEXEC) == -1) {
        FATAL("Failed setting CLOEXEC for master pty");
    }
    if (fcntl(s_SlaveFd, F_SETFD, FD_CLOEXEC) == -1) {
        FATAL("Failed setting CLOEXEC for slave pty");
    }

    pid_t pid = fork();
    if (pid == 0) {
        // Child process
        if (dup2(s_SlaveFd, STDIN_FILENO) == -1) {
            FATAL_CHILD("Failed piping stdin: {}", std::strerror(errno));
        }
        if (dup2(s_SlaveFd, STDOUT_FILENO) == -1) {
            FATAL_CHILD("Failed piping stdout: {}", std::strerror(errno));
        }
        if (dup2(s_SlaveFd, STDERR_FILENO) == -1) {
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

        if (ioctl(s_SlaveFd, TIOCSCTTY, 0) == -1) {
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
        s_TermProcessPid = pid;
    } else {
        FATAL("Failed forking process: {}", strerror(errno));
    }

    SPDLOG_DEBUG("Opened pty");
}

void Terminal::close() {
    s_ShouldClose.store(true, std::memory_order_relaxed);
    ::close(s_MasterFd);
    ::close(s_SlaveFd);
    kill(s_TermProcessPid, SIGKILL);
    SPDLOG_DEBUG("Closed pty");
}

std::vector<uint8_t> Terminal::read() {
    constexpr size_t BUF_SIZE = 65535;

    std::vector<uint8_t> buf(BUF_SIZE);
    int bytesRead = ::read(s_MasterFd, buf.data(), BUF_SIZE);

    if (bytesRead >= 0) {
        buf.resize(bytesRead);
    } else {
        throw TerminalReadException();
    }

    return buf;
}

void Terminal::write(codepoint_t codepoint) {
    ::write(s_MasterFd, &codepoint, 1);
    SPDLOG_TRACE("Written '{}' (0x{:x}) to terminal", (char)codepoint,
                 codepoint);
}

bool Terminal::shouldClose() {
    return s_ShouldClose.load(std::memory_order_relaxed);
}

void Terminal::getBuf(std::function<void(const TerminalBuf&)> cb) {
    std::shared_lock lock(s_BufMutex);
    cb(s_Buf);
}

void Terminal::getBufMut(std::function<void(TerminalBuf&)> cb) {
    std::unique_lock lock(s_BufMutex);
    cb(s_Buf);
}
