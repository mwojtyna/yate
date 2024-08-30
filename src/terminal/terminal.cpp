#include "terminal.hpp"
#include "../utils.hpp"
#include <cerrno>
#include <csignal>
#include <cstdlib>
#include <cstring>
#include <fcntl.h>
#include <pty.h>
#include <pwd.h>
#include <spdlog/spdlog.h>
#include <sys/ioctl.h>
#include <unistd.h>

#define FATAL_CHILD(...)                                                       \
    SPDLOG_ERROR(__VA_ARGS__);                                                 \
    kill(getppid(), SIGTERM);

TerminalData Terminal::s_Data;

void Terminal::open() {
    // Child process
    if (openpty(&s_Data.masterFd, &s_Data.slaveFd, nullptr, nullptr, nullptr)) {
        FATAL("Failed to open pty: {}", strerror(errno));
    }

    if (fcntl(s_Data.masterFd, F_SETFD, FD_CLOEXEC) == -1) {
        FATAL("Failed setting CLOEXEC for master pty");
    }
    if (fcntl(s_Data.slaveFd, F_SETFD, FD_CLOEXEC) == -1) {
        FATAL("Failed setting CLOEXEC for slave pty");
    }

    pid_t pid = fork();
    if (pid == 0) {
        if (dup2(s_Data.slaveFd, STDIN_FILENO) == -1) {
            FATAL_CHILD("Failed piping stdin: {}", std::strerror(errno));
        }
        if (dup2(s_Data.slaveFd, STDOUT_FILENO) == -1) {
            FATAL_CHILD("Failed piping stdout: {}", std::strerror(errno));
        }
        if (dup2(s_Data.slaveFd, STDERR_FILENO) == -1) {
            FATAL_CHILD("Failed piping stderr: {}", std::strerror(errno));
        }

        char* home = getenv("HOME");
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

        if (ioctl(s_Data.slaveFd, TIOCSCTTY, 0) == -1) {
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
        s_Data.termProcessPid = pid;
    } else {
        FATAL("Failed forking process: {}", strerror(errno));
    }

    SPDLOG_DEBUG("Opened pty");
}

void Terminal::close() {
    ::close(s_Data.masterFd);
    ::close(s_Data.slaveFd);
    kill(s_Data.termProcessPid, SIGKILL);
    SPDLOG_DEBUG("Closed pty");
}

std::vector<char> Terminal::read() {
    std::vector<char> buf(1024);

    SPDLOG_DEBUG("Reading...");
    int bytesRead = ::read(s_Data.masterFd, buf.data(), 1024);
    if (bytesRead == -1) {
        SPDLOG_ERROR("Failed reading from pty");
    } else if (bytesRead == 0) {
        SPDLOG_DEBUG("EOF");
    } else {
        SPDLOG_DEBUG("Read {} bytes", bytesRead);
    }

    return buf;
}

void Terminal::write() {
    const char* buf = "chuj";
    ::write(s_Data.masterFd, buf, strlen(buf));
}
