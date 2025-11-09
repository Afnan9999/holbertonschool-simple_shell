cat << 'EOF' > shell.c
#include "shell.h"
#include <stdio.h>
#include <stdlib.h>
#include <unistd.h>
#include <string.h>
#include <sys/wait.h>

extern char **environ;

int main(void)
{
    char *line = NULL;
    size_t len = 0;
    ssize_t nread;
    char *args[2];
    pid_t pid;
    char *cmd_path;

    while (1)
    {
        if (isatty(STDIN_FILENO))
            write(STDOUT_FILENO, "$ ", 2);

        nread = getline(&line, &len, stdin);
        if (nread == -1)
        {
            free(line);
            exit(0);
        }

        line[strcspn(line, "\n")] = '\0';

        if (strcmp(line, "env") == 0)
        {
            print_env();
            continue;
        }

        if (strcmp(line, "exit") == 0)
        {
            free(line);
            exit(0);
        }

        cmd_path = find_command_path(line);
        if (!cmd_path)
        {
            perror("Command not found");
            continue;
        }

        args[0] = cmd_path;
        args[1] = NULL;

        pid = fork();
        if (pid == -1)
        {
            perror("fork");
            free(cmd_path);
            free(line);
            exit(1);
        }

        if (pid == 0)
        {
            if (execve(cmd_path, args, environ) == -1)
            {
                perror("execve");
                exit(127);
            }
        }
        else
        {
            wait(NULL);
            free(cmd_path);
        }
    }

    free(line);
    return (0);
}
EOF
