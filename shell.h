cat << 'EOF' > shell.h
#ifndef SHELL_H
#define SHELL_H

void print_env(void);
char *find_command_path(char *command);

#endif /* SHELL_H */
EOF
