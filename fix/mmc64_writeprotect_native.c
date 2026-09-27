#include <sys/types.h>
#include <sys/wait.h>
#include <sys/stat.h>

#include <errno.h>
#include <fcntl.h>
#include <stdio.h>
#include <stdlib.h>
#include <string.h>
#include <unistd.h>

#define MMC64_PATH "/sbin/mmc64"
#define DEVICE_PATH "/dev/block/mmcblk0"
#define LOG_PATH "/tmp/mmc64_writeprotect_native.log"

extern char **environ;

static void log_message(const char *message)
{
    FILE *fp;

    printf("[mmc64-native] %s\n", message);
    fflush(stdout);

    fp = fopen(LOG_PATH, "a");
    if (fp != NULL) {
        fprintf(fp, "[mmc64-native] %s\n", message);
        fclose(fp);
    }
}

static void log_errno_message(const char *prefix)
{
    char buffer[512];

    snprintf(
        buffer,
        sizeof(buffer),
        "%s: errno=%d (%s)",
        prefix,
        errno,
        strerror(errno)
    );

    log_message(buffer);
}

static int check_path(const char *path)
{
    struct stat st;

    if (stat(path, &st) != 0) {
        log_errno_message("stat failed");
        return -1;
    }

    if (!S_ISREG(st.st_mode)) {
        log_message("mmc64 is not a regular file");
        return -1;
    }

    if (access(path, X_OK) != 0) {
        log_errno_message("mmc64 is not executable");
        return -1;
    }

    return 0;
}

static int check_device(const char *path)
{
    struct stat st;

    if (stat(path, &st) != 0) {
        log_errno_message("device stat failed");
        return -1;
    }

    if (!S_ISBLK(st.st_mode)) {
        log_message("device is not a block device");
        return -1;
    }

    return 0;
}

static int run_mmc64(char *const argv[])
{
    pid_t pid;
    int status;
    char command[1024];
    size_t pos;
    int i;

    command[0] = '\0';
    pos = 0;

    for (i = 0; argv[i] != NULL; ++i) {
        int written;

        written = snprintf(
            command + pos,
            sizeof(command) - pos,
            "%s%s",
            (i == 0) ? "" : " ",
            argv[i]
        );

        if (written < 0) {
            log_message("failed to build command log");
            return 1;
        }

        if ((size_t)written >= sizeof(command) - pos) {
            log_message("command log buffer overflow");
            return 1;
        }

        pos += (size_t)written;
    }

    log_message("========================================");
    log_message("executing mmc64:");
    log_message(command);

    pid = fork();

    if (pid < 0) {
        log_errno_message("fork failed");
        return 1;
    }

    if (pid == 0) {
        if (setenv("PATH", "/sbin:/system/bin", 1) != 0) {
            _exit(126);
        }

        if (setenv("LD_LIBRARY_PATH", "/sbin:/system/lib:/vendor/lib", 1) != 0) {
            _exit(126);
        }

        execve(MMC64_PATH, argv, environ);

        _exit(127);
    }

    for (;;) {
        pid_t result;

        result = waitpid(pid, &status, 0);

        if (result == pid) {
            break;
        }

        if (result < 0) {
            if (errno == EINTR) {
                continue;
            }

            log_errno_message("waitpid failed");
            return 1;
        }
    }

    if (WIFEXITED(status)) {
        int exit_code;

        exit_code = WEXITSTATUS(status);

        {
            char buffer[256];

            snprintf(
                buffer,
                sizeof(buffer),
                "mmc64 exited normally: code=%d",
                exit_code
            );

            log_message(buffer);
        }

        return exit_code;
    }

    if (WIFSIGNALED(status)) {
        int signal_number;

        signal_number = WTERMSIG(status);

        {
            char buffer[256];

            snprintf(
                buffer,
                sizeof(buffer),
                "mmc64 terminated by signal=%d",
                signal_number
            );

            log_message(buffer);
        }

        return 128 + signal_number;
    }

    log_message("mmc64 ended with unknown wait status");
    return 1;
}

static int run_first(void)
{
    char *argv[] = {
        "mmc64",
        "writeprotect",
        "user",
        "set",
        "none",
        "0",
        "409600",
        DEVICE_PATH,
        NULL
    };

    return run_mmc64(argv);
}

static int run_second(void)
{
    char *argv[] = {
        "mmc64",
        "writeprotect",
        "user",
        "set",
        "none",
        "507904",
        "3932160",
        DEVICE_PATH,
        NULL
    };

    return run_mmc64(argv);
}

static int run_all(void)
{
    int result;

    result = run_first();

    if (result != 0) {
        log_message("first mmc64 command failed");
        log_message("second mmc64 command was not executed");
        return result;
    }

    log_message("first mmc64 command succeeded");

    result = run_second();

    if (result != 0) {
        log_message("second mmc64 command failed");
        return result;
    }

    log_message("second mmc64 command succeeded");

    return 0;
}

static int run_custom(int argc, char **argv)
{
    char **mmc_argv;
    int i;

    if (argc <= 1) {
        return run_all();
    }

    mmc_argv = (char **)calloc(
        (size_t)argc + 1,
        sizeof(char *)
    );

    if (mmc_argv == NULL) {
        log_message("calloc failed");
        return 1;
    }

    mmc_argv[0] = "mmc64";

    for (i = 1; i < argc; ++i) {
        mmc_argv[i] = argv[i];
    }

    mmc_argv[argc] = NULL;

    i = run_mmc64(mmc_argv);

    free(mmc_argv);

    return i;
}

int main(int argc, char **argv)
{
    int result;

    unlink(LOG_PATH);

    log_message("========================================");
    log_message("mmc64 native writeprotect launcher");
    log_message("Android 5.1 TWRP recovery");
    log_message("========================================");

    if (check_path(MMC64_PATH) != 0) {
        log_message("ERROR: /sbin/mmc64 cannot be used");
        return 1;
    }

    if (check_device(DEVICE_PATH) != 0) {
        log_message("ERROR: /dev/block/mmcblk0 cannot be used");
        return 1;
    }

    log_message("mmc64 path: " MMC64_PATH);
    log_message("device path: " DEVICE_PATH);

    if (argc == 1) {
        log_message("mode: all");
        result = run_all();
    } else if (argc == 2 && strcmp(argv[1], "first") == 0) {
        log_message("mode: first");
        result = run_first();
    } else if (argc == 2 && strcmp(argv[1], "second") == 0) {
        log_message("mode: second");
        result = run_second();
    } else if (argc == 2 && strcmp(argv[1], "all") == 0) {
        log_message("mode: all");
        result = run_all();
    } else {
        log_message("mode: custom");
        result = run_custom(argc, argv);
    }

    if (result == 0) {
        log_message("========================================");
        log_message("completed successfully");
        log_message("========================================");
    } else {
        log_message("========================================");
        log_message("completed with failure");
        log_message("========================================");
    }

    return result;
}
