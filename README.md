# 0x16. C – Simple Shell

## Overview

This project is a simplified implementation of a UNIX command line interpreter written in C.  
It reproduces core behaviors of `/bin/sh` to demonstrate a deep understanding of process creation, program execution, environment handling, and system-level programming.

The shell supports interactive and non-interactive modes, executes commands using the system `PATH`, and follows all constraints defined by the Holberton School project specifications.

---

## Objectives

This project demonstrates practical knowledge of:

- How a UNIX shell operates internally
- Process management using `fork` and `waitpid`
- Program execution with `execve`
- Environment variable handling
- PATH resolution
- Interactive vs non-interactive execution
- Proper error reporting and exit statuses
- Memory-safe C programming
- Writing clean, maintainable, Betty-compliant code

---

## Features

### Supported Functionality

- Displays a prompt and waits for user input
- Reads input using `getline`
- Parses commands into tokens
- Executes programs found in `PATH`
- Executes commands using absolute and relative paths
- Implements required built-in commands
- Handles end-of-file (Ctrl + D)
- Produces error output identical to `/bin/sh`

### Built-in Commands

- **`exit`**  
  Exits the shell and returns the status of the last executed command.

- **`env`**  
  Prints the current environment variables.

---

## Execution Modes

### Interactive Mode

When the shell is run in a terminal:

```bash
$ ./hsh
($) /bin/ls
hsh main.c
($) exit
$

