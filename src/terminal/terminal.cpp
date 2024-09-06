#include "types.hpp"
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

// TODO: Divide window into terminal rows and columns
void Terminal::open(int windowWidth, int windowHeight) {
    if (openpty(&m_MasterFd, &m_SlaveFd, nullptr, nullptr, nullptr)) {
        FATAL("Failed to open pty: {}", strerror(errno));
    }

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

        const char* argv[] = {"-bash", nullptr};
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

termbuf_t Terminal::read() {
    constexpr size_t BUF_SIZE = 65535;

    termbuf_t buf(BUF_SIZE);
    int bytesRead = ::read(m_MasterFd, buf.data(), BUF_SIZE);

    if (bytesRead >= 0) {
        buf.resize(bytesRead);
    } else {
        throw TerminalReadException();
    }

    return buf;
}

void Terminal::write(uint8_t buf[], size_t len) {
    ::write(m_MasterFd, buf, len);
}

bool Terminal::shouldClose() {
    return m_ShouldClose.load(std::memory_order_relaxed);
}
