cat > path_handler.c << 'EOF'
#include <stdlib.h>
#include <string.h>
#include <unistd.h>
#include <stdio.h>

char *find_command_path(char *command)
{
    char *path = getenv("PATH");
    char *token, *full_path;
    size_t len;

    if (!path)
        return (NULL);

    path = strdup(path);
    token = strtok(path, ":");
    while (token)
    {
        len = strlen(token) + strlen(command) + 2;
        full_path = malloc(len);
        if (!full_path)
        {
            free(path);
            return (NULL);
        }

        snprintf(full_path, len, "%s/%s", token, command);
        if (access(full_path, X_OK) == 0)
        {
            free(path);
            return (full_path);
        }

        free(full_path);
        token = strtok(NULL, ":");
    }
    free(path);
    return (NULL);
}
EOF
