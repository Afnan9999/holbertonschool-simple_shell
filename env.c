cat << 'EOF' > env.c
#include <stdio.h>

extern char **environ;

void print_env(void)
{
    int i = 0;
    while (environ[i])
    {
        printf("%s\n", environ[i]);
        i++;
    }
}
EOF
