#include "shell.h"

/**
 * get_path_variable - Retrieves the PATH environment variable
 *
 * Return: PATH value or NULL if not found
 */
char *get_path_variable(void)
{
	int i = 0;

	while (environ && environ[i])
	{
		if (strncmp(environ[i], "PATH=", 5) == 0)
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
 * handle_builtins - Handles built-in commands
 * @buffer: Input command
 *
 * Return: 1 if built-in handled, 0 otherwise
 */
int handle_builtins(char *buffer)
{
	if (strcmp(buffer, "exit") == 0)
	{
		free(buffer);
		exit(0);
	}

	if (strcmp(buffer, "env") == 0)
	{
		print_env();
		return (1);
	}

	return (0);
}

/**
 * execute_command - Executes external command
 * @args: Command arguments
 * @command_path: Full path to command
 */
void execute_command(char **args, char *command_path)
{
	pid_t pid;
	int status;

	pid = fork();
	if (pid == -1)
	{
		perror("Error:");
		return;
	}

	if (pid == 0)
	{
		if (execve(command_path, args, environ) == -1)
		{
			perror("./shell");
			exit(2);
		}
	}
	else
	{
		wait(&status);
	}
}

/**
 * read_input - Reads input from user
 * @buffer: Buffer to store input
 * @bufsize: Size of buffer
 * @is_interactive: Whether in interactive mode
 *
 * Return: Number of characters read
 */
int read_input(char **buffer, size_t *bufsize, int is_interactive)
{
	ssize_t characters;

	if (is_interactive)
		write(STDOUT_FILENO, ":) ", 3);

	characters = getline(buffer, bufsize, stdin);
	if (characters == -1)
	{
		if (is_interactive)
			write(STDOUT_FILENO, "\n", 1);
		return (-1);
	}

	(*buffer)[characters - 1] = '\0';
	return (characters);
}

/**
 * process_command - Processes a single command
 * @buffer: Command string
 */
void process_command(char *buffer)
{
	char **args;
	char *command_path;

	if (strlen(buffer) == 0)
		return;

	if (handle_builtins(buffer))
		return;

	args = tokenize_command(buffer);
	command_path = find_command_path(args[0]);

	if (!command_path)
	{
		fprintf(stderr, "%s: command not found\n", args[0]);
		free(args);
		return;
	}

	execute_command(args, command_path);
	free(command_path);
	free(args);
}

/**
 * main - Simple shell entry point
 *
 * Return: Exit status
 */
int main(void)
{
	char *buffer = NULL;
	size_t bufsize = 0;
	int is_interactive = isatty(STDIN_FILENO);
	int status = 0;

	while (1)
	{
		if (read_input(&buffer, &bufsize, is_interactive) == -1)
			break;

		process_command(buffer);
	}

	free(buffer);
	return (status);
}
