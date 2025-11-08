#include "shell.h"

int main(void)
{
    char *line = NULL;
    size_t len = 0;
    ssize_t nread;
    char *args[2];
    pid_t pid;

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

        line[strcspn(line, "\n")] = '\0';  // إزالة السطر الجديد
        if (line[0] == '\0')
            continue;

        args[0] = line;
        args[1] = NULL;

        pid = fork();
        if (pid == -1)
        {
            perror("fork");
            free(line);
            exit(1);
        }

        if (pid == 0)
        {
            execve(args[0], args, environ);
            perror(args[0]);
            exit(127);
        }
        else
        {
            wait(NULL);
        }
    }

    free(line);
    return 0;
}

#ifndef SHELL_H
#define SHELL_H

#include <stdio.h>
#include <stdlib.h>
#include <unistd.h>
#include <string.h>
#include <sys/wait.h>

extern char **environ;

#endif


