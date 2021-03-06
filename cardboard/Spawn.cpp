#include "Spawn.h"

template <typename... Args>
static void ignore(Args&&...)
{
}

std::error_code spawn(std::function<int()> fn)
{
    // credits: http://www.lubutu.com/code/spawning-in-unix
    int fd[2];
    if (pipe(fd) == -1) {
        int err = errno;
        return std::error_code(err, std::generic_category());
    }
    if (fork() == 0) {
        close(fd[0]);
        fcntl(fd[1], F_SETFD, FD_CLOEXEC);

        setsid();
        int status = fn();

        ignore(write(fd[1], &errno, sizeof(errno)));
        exit(status);
    }

    close(fd[1]);

    int code = 0;
    if (!read(fd[0], &code, sizeof(code))) {
        code = 0;
    }
    close(fd[0]);
    return std::error_code(code, std::generic_category());
}
