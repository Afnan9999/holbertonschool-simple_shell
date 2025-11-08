#include <stdio.h>
#include <stdlib.h>
#include <unistd.h>
#include <string.h>
#include <sys/wait.h>
#include <sys/stat.h>

extern char **environ;

/**
 * get_path_variable - Retrieves the PATH environment variable
 *
 * Return: PATH value or NULL if not found
 */
char *get_path_variable(void)
{
	int i = 0;
	char *path_prefix = "PATH=";

	while (environ && environ[i])
	{
		if (strncmp(environ[i], path_prefix, 5) == 0)
			return (environ[i] + 5);
		i++;
	}
	return (NULL);
}

/**
 * find_command_path - Finds command in PATH directories
 * @command: Command to search for
 *
 * Return: Full path if found, NULL otherwise
 */
char *find_command_path(char *command)
{
	char *path, *token, *full_path, *dup_path;
	struct stat st;

	if (access(command, X_OK) == 0)
		return (strdup(command));

	path = get_path_variable();
	if (!path)
		return (NULL);

	dup_path = strdup(path);
	if (!dup_path)
		return (NULL);

	token = strtok(dup_path, ":");
	while (token)
	{
		full_path = malloc(strlen(token) + strlen(command) + 2);
		if (!full_path)
		{
			perror("Allocation error");
			free(dup_path);
			exit(EXIT_FAILURE);
		}

		sprintf(full_path, "%s/%s", token, command);
		if (stat(full_path, &st) == 0 && access(full_path, X_OK) == 0)
		{
			free(dup_path);
			return (full_path);
		}

		free(full_path);
		token = strtok(NULL, ":");
	}
	free(dup_path);
	return (NULL);
}

/**
 * tokenize_command - Splits command into arguments
 * @buffer: Input string to tokenize
 *
 * Return: Array of arguments
 */
char **tokenize_command(char *buffer)
{
	char *token;
	char **args;
	int i = 0;

	args = malloc(10 * sizeof(char *));
	if (!args)
	{
		perror("Allocation error");
		exit(EXIT_FAILURE);
	}

	token = strtok(buffer, " ");
	while (token != NULL)
	{
		args[i++] = token;
		token = strtok(NULL, " ");
	}
	args[i] = NULL;

	return (args);
}

/**
 * print_env - Prints environment variables
 */
void print_env(void)
{
	int i = 0;

	if (!environ)
		return;

	while (environ[i])
	{
		printf("%s\n", environ[i]);
		i++;
	}
}

/**
 * main - Simple shell entry point
 *
 * Return: Exit status
 */
int main(void)
{
	char *buffer = NULL, *command_path;
	char **args;
	size_t bufsize = 0;
	ssize_t characters;
	pid_t pid;
	int status;
	int is_interactive = isatty(STDIN_FILENO);

	while (1)
	{
		if (is_interactive)
			write(STDOUT_FILENO, ":) ", 3);

		characters = getline(&buffer, &bufsize, stdin);
		if (characters == -1)
		{
			if (is_interactive)
				write(STDOUT_FILENO, "\n", 1);
			break;
		}

		buffer[characters - 1] = '\0';

		if (strlen(buffer) == 0)
			continue;

		if (strcmp(buffer, "exit") == 0)
		{
			free(buffer);
			exit(0);
		}

		if (strcmp(buffer, "env") == 0)
		{
			print_env();
			continue;
		}

		args = tokenize_command(buffer);
		command_path = find_command_path(args[0]);

		if (!command_path)
		{
			fprintf(stderr, "%s: command not found\n", args[0]);
			free(args);
			continue;
		}

		pid = fork();
		if (pid == -1)
		{
			perror("Error:");
			free(args);
			free(command_path);
			break;
		}
		if (pid == 0)
		{
			if (execve(command_path, args, environ) == -1)
			{
				perror("./shell");
				free(args);
				free(command_path);
				exit(2);
			}
		}
		else
		{
			wait(&status);
			if (WIFEXITED(status))
				status = WEXITSTATUS(status);
		}

		free(command_path);
		free(args);
	}

	free(buffer);
	return (status);
}
