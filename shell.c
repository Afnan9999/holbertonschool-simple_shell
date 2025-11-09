#include "shell.h"
#include <stdio.h>
#include <stdlib.h>
#include <string.h>
#include <unistd.h>
#include <sys/wait.h>

extern char **environ;

int main(void)
{
    char *line = NULL, *cmd_path = NULL;
    size_t len = 0;
    char *args[2];
    ssize_t read;

    while (1)
    {
        write(STDOUT_FILENO, "$ ", 2);
        read = getline(&line, &len, stdin);
        if (read == -1)
            break;

        line[strcspn(line, "\n")] = '\0';

        if (strcmp(line, "exit") == 0)
            break;

        args[0] = line;
        args[1] = NULL;

        cmd_path = find_command_path(line);
        if (cmd_path)
        {
            if (fork() == 0)
            {
                execve(cmd_path, args, environ);
                perror("execve");
                exit(EXIT_FAILURE);
            }
            else
            {
                wait(NULL);
                free(cmd_path);
            }
        }
        else
        {
            write(STDOUT_FILENO, "Command not found: No such file or directory\n", 46);
        }
    }

    free(line);
    return (0);
}
