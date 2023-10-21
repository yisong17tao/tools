#include <sys/ptrace.h>
#include <sys/types.h>
#include <sys/wait.h>
#include <sys/user.h>
#include <unistd.h>
#include <stdio.h>

int main(int argc, char *argv[])
{
    if (argc < 2) {
        printf("Usage: %s <command> [args...]\n", argv[0]);
        return 1;
    }

    pid_t child;
    long orig_rax;
    struct user_regs_struct regs;

    child = fork();
    if (child == 0) {
        // 子进程
        ptrace(PTRACE_TRACEME, 0, NULL, NULL);
        execvp(argv[1], &argv[1]);
    } else {
        // 父进程
        wait(NULL);
        while (1) {
            ptrace(PTRACE_SYSCALL, child, NULL, NULL);
            wait(NULL);
            ptrace(PTRACE_GETREGS, child, NULL, &regs);
            orig_rax = regs.orig_rax;
            if (orig_rax == -1 || orig_rax == 231)
                break;
            printf("System call number: %ld\n", orig_rax);
        }
    }

    return 0;
}
