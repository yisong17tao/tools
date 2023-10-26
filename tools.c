#include <sys/ptrace.h>
#include <sys/types.h>
#include <sys/wait.h>
#include <sys/user.h>
#include <unistd.h>
#include <stdio.h>
#include <string.h>
#include <stdlib.h>

void PrintHelp()
{
    printf("This is a tool using ptrace!\n");
}

char *strdup(const char *s)
{
    size_t len = strlen(s) + 1;
    char *copy = (char *)malloc(len);
    if (copy != NULL)
    {
        memcpy(copy, s, len);
    }
    return copy;
}

int comline(int argc, char *argv[])
{
    if (argc < 2)
    {
        printf("Usage: %s <command> [args...]\n", argv[0]);
        return 0;
    }
    else if (strcmp(argv[1], "-h") || strcmp(argv[1], "--help") == 0)
    {
        PrintHelp();
        return 1;
    }
}

char **Read_SYSCALL_List(const char *filename, int *lineCount, int *count)
{
    FILE *file = fopen(filename, "r");
    if (file == NULL)
    {
        printf("Failed to open file: %s\n", filename);
        return NULL;
    }

    // count the number of file lines.
    char buffer[256];
    while (fgets(buffer, sizeof(buffer), file) != NULL)
    {
        (*count)++;
    }
    *lineCount = *count;

    // allocate space of a string array.
    char **lines = (char **)malloc(*count * sizeof(char *));
    if (lines == NULL)
    {
        printf("Memory allocation failed.\n");
        fclose(file);
        return NULL;
    }

    // relocate the file pointer to the start.
    fseek(file, 0, SEEK_SET);

    // reads the contents of a file line by line into an string array.
    int i = 0;
    while (fgets(buffer, sizeof(buffer), file) != NULL)
    {
        int length = strlen(buffer);
        if (buffer[length - 1] == '\n')
        {
            buffer[length - 1] = '\0'; // remove newline characters
        }
        lines[i] = strdup(buffer);
        i++;
    }
    fclose(file);

    return lines;
}

void FreeStringArray(char **array, int count)
{
    if (array == NULL)
    {
        return;
    }

    for (int i = 0; i < count; i++)
    {
        if (array[i] != NULL)
        {
            free(array[i]);
        }
    }

    free(array);
}

pid_t Create_Child_And_Ptrace(char *argv[])
{
    pid_t child;
    long orig_rax;
    struct user_regs_struct regs;
    int lineCount=0;
    int count=0;
    char **lines = Read_SYSCALL_List("syscall_list.txt", &lineCount, &count);

    // printf("debug:lineCount:%d count:%d\n",lineCount, count);

    child = fork();
    if (child == 0)
    {
        // child process
        ptrace(PTRACE_TRACEME, 0, NULL, NULL);
        execvp(argv[1], &argv[1]);
    }
    else
    {
        // parent process
        wait(NULL);
        while (1)
        {
            ptrace(PTRACE_SYSCALL, child, NULL, NULL);
            wait(NULL);
            ptrace(PTRACE_GETREGS, child, NULL, &regs);
            orig_rax = regs.orig_rax;
            if (orig_rax == -1 || orig_rax == 231)
                break;
            printf("System call number: %s\n", lines[orig_rax]);
        }
    }
    // FreeStringArray(lines, count);
    return child;
}

int main(int argc, char *argv[])
{
    comline(argc, argv);
    Create_Child_And_Ptrace(argv);
    return 0;
}
