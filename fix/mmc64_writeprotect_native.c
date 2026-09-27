#include <sys/types.h>
#include <sys/wait.h>

#include <errno.h>
#include <stdio.h>
#include <stdlib.h>
#include <string.h>
#include <unistd.h>

#define MMC64_PATH "/sbin/mmc64"
#define DEVICE_PATH "/dev/block/mmcblk0"

static int run_mmc64(
        const char *start,
        const char *count)
{
    pid_t pid;
    int status;

    char *const argv[] = {
        "mmc64",
        "writeprotect",
        "user",
        "set",
        "none",
        (char *)start,
        (char *)count,
        DEVICE_PATH,
        NULL
    };

    char *const envp[] = {
        "PATH=/sbin:/system/bin",
        "LD_LIBRARY_PATH=/sbin:/system/lib",
        NULL
    };

    pid = fork();

    if (pid < 0) {
        return 1;
    }

    if (pid == 0) {
        execve(MMC64_PATH, argv, envp);
        _exit(127);
    }

    for (;;) {
        pid_t r;

        r = waitpid(pid, &status, 0);

        if (r == pid) {
            break;
        }

        if (r < 0 && errno == EINTR) {
            continue;
        }

        return 1;
    }

    if (WIFEXITED(status)) {
        return WEXITSTATUS(status);
    }

    if (WIFSIGNALED(status)) {
        return 128 + WTERMSIG(status);
    }

    return 1;
}

int main(void)
{
    int result;

    result = run_mmc64("0", "409600");

    if (result != 0) {
        return result;
    }

    result = run_mmc64("507904", "3932160");

    return result;
}
