# Simple Shell

A basic UNIX shell written in C that understands and executes commands.

## What It Does
- Runs programs like `/bin/ls`
- Works when you type commands or pipe them
- Finds programs using PATH
- Shows errors like real shell
- Uses memory properly

## Requirements
- Ubuntu 20.04
- GCC compiler
- Betty code style
- No memory leaks

## How to Build
```bash
gcc -Wall -Werror -Wextra -pedantic -std=gnu89 *.c -o hsh
