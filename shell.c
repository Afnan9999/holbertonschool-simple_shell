#ifndef SHELL_H
#define SHELL_H

#include <stdio.h>
#include <stdlib.h>
#include <unistd.h>
#include <string.h>
#include <sys/wait.h>
#include <sys/stat.h>

extern char **environ;

/* Function prototypes */
char *get_path_variable(void);
char *find_command_path(char *command);
char **tokenize_command(char *buffer);
void print_env(void);
int handle_builtins(char *buffer);
void execute_command(char **args, char *command_path);
int read_input(char **buffer, size_t *bufsize, int is_interactive);
void process_command(char *buffer);

#endif /* SHELL_H */
