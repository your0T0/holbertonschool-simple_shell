# 🐚 0x16. C – Simple Shell

> *“The shell is the bridge between the user and the operating system.”*

---

## 👥 Authors

**Manar** & **Yara**  
*Simple Shell Developers*

---

## 📋 Table of Contents

- Project Overview  
- Learning Objectives  
- What Is a Shell?  
- Shell Architecture  
- Execution Flow  
- Interactive vs Non-Interactive Mode  
- Input Handling  
- Command Parsing  
- Built-in Commands  
- PATH Resolution  
- Process Management  
- Error Handling  
- Exit Status Management  
- Memory Management  
- File Structure  
- Compilation & Usage  
- Limitations  
- Coding Standards  
- Educational Purpose  

---

## 🧭 Project Overview

This project is a simplified implementation of a UNIX command line interpreter written in **C**, developed as part of the **Holberton School Low-Level Programming curriculum**.

The goal of this project is to reproduce the core behavior of a UNIX shell such as `/bin/sh` while respecting strict constraints on allowed system calls, memory usage, and coding style.

The shell reads user input, parses commands, resolves executable paths, creates child processes, and executes programs using low-level system calls.

This project focuses on understanding how shells work internally rather than building a full-featured production shell.

---

## 🎯 Learning Objectives

By completing this project, we demonstrate understanding of:

- How a UNIX shell operates internally  
- How commands are read and interpreted  
- Process creation using `fork`  
- Program execution using `execve`  
- Process synchronization using `wait`  
- Environment variable handling  
- PATH parsing and executable lookup  
- Interactive and non-interactive shell behavior  
- Exit status propagation  
- Memory-safe programming in C  
- Writing modular, Betty-compliant code  

---

## 🐚 What Is a Shell?

A shell is a program that provides an interface between the user and the operating system.

Its responsibilities include:
- Reading user commands
- Parsing command arguments
- Locating executable files
- Creating child processes
- Executing programs
- Handling errors and exit statuses

Without a shell, users would need to interact directly with system calls, which is impractical.

---

## 🏗️ Shell Architecture

The shell follows a loop-based architecture:

1. Display a prompt (interactive mode only)
2. Read input from standard input
3. Validate and sanitize input
4. Parse input into tokens
5. Handle built-in commands
6. Resolve command paths
7. Create a child process
8. Execute the command
9. Wait for execution to finish
10. Store exit status
11. Repeat until exit or end-of-file

This loop continues until the shell terminates.

---

## 🔁 Execution Flow

For each command, the shell performs the following steps:

1. **Read Input**  
   Input is read using a custom `my_getline` function to control memory usage and comply with project constraints.

2. **Sanitize Input**  
   Newline characters are removed. Empty or whitespace-only input is ignored.

3. **Parse Command**  
   The input is tokenized into a command and arguments, stored in a NULL-terminated argument vector.

4. **Handle Built-ins**  
   Built-in commands such as `exit` and `env` are executed internally without creating a child process.

5. **Resolve Executable**  
   Commands with `/` are treated as paths. Other commands are searched for in the PATH environment variable.

6. **Execute Command**  
   A child process is created using `fork`, and the command is executed using `execve`.

7. **Update Exit Status**  
   The shell stores the exit status of the last executed command.

---

## 🖥️ Interactive vs Non-Interactive Mode

### Interactive Mode
The shell is attached to a terminal and displays a prompt.

Example:

$ ./hsh
($) ls
($) exit

### Non-Interactive Mode
The shell reads input from a pipe or file and does not display a prompt.

Example:
$ echo "ls" | ./hsh

The mode is detected using `isatty`.

---

## ⌨️ Input Handling

- Input is read line by line
- End-of-file (`Ctrl + D`) is handled gracefully
- Memory is reused when possible
- Whitespace-only input is ignored

---

## 🔍 Command Parsing

- Commands are split using spaces and tabs as delimiters
- The first token is the command name
- Remaining tokens are treated as arguments
- A NULL-terminated `argv` array is prepared for `execve`

This behavior matches how UNIX programs receive arguments.

---

## 🔧 Built-in Commands

### `exit`
- Terminates the shell
- Accepts an optional numeric exit status
- Invalid arguments produce an error and exit with status `2`

### `env`
- Prints all environment variables available to the process

Built-ins are executed without calling `fork`.

---

## 🧭 PATH Resolution

If a command does not contain a `/`, the shell:

1. Retrieves the PATH environment variable
2. Splits it into directories
3. Appends the command name to each directory
4. Checks if the resulting path:
   - Exists
   - Is a regular file
   - Has execute permission

The shell only calls `fork` if a valid executable is found.

---

## 👶 Process Management

- `fork` creates a child process
- `execve` replaces the child process image
- `waitpid` synchronizes execution
- Exit status is collected by the parent process

---

## ❌ Error Handling

The shell:
- Matches `/bin/sh` error message format
- Prints errors to standard error
- Handles command not found cases
- Handles invalid exit arguments
- Uses correct exit codes such as `127` and `2`

Error messages include:
- Shell name
- Command count
- Command name

---

## 🔢 Exit Status Management

- The exit status of the last command is stored
- Built-ins update the exit status correctly
- The shell exits using the last known status

---

## 🧠 Memory Management

- Dynamically allocated memory is freed
- No memory leaks are allowed
- Buffers are reused where possible
- Child processes do not leak memory due to `execve`

---

## 🗂️ File Structure and Responsibilities

This project is organized into multiple source files, each responsible for a specific part of the shell’s behavior.  
This modular design improves readability, maintainability, and debugging.

---

### 📄 `main.c`

This file contains the **entry point of the shell** and controls the main execution loop.

Responsibilities:
- Detects whether the shell is running in interactive or non-interactive mode using `isatty`
- Displays the prompt in interactive mode
- Reads user input using the custom input handler
- Ignores empty or whitespace-only commands
- Parses input into arguments
- Handles built-in commands
- Coordinates command execution
- Tracks command count
- Stores and updates the last exit status
- Terminates the shell gracefully on `exit` or end-of-file

This file acts as the **central controller** of the shell.

---

### 📄 `getline.c`

This file implements a **custom line-reading function** used instead of the standard `getline`.

Responsibilities:
- Reads input from a file descriptor
- Dynamically manages memory for input buffers
- Handles partial reads and buffering
- Supports both interactive and non-interactive input
- Detects end-of-file conditions correctly

Using a custom getline implementation allows:
- Better control over memory
- Compliance with project constraints
- Consistent behavior across execution modes

---

### 📄 `helpers.c`

This file contains **utility helper functions** used throughout the shell.

Typical responsibilities include:
- Custom string functions (comparison, duplication, concatenation)
- Token manipulation helpers
- Numeric conversion helpers
- Reusable logic shared between files

Keeping helpers separate:
- Avoids code duplication
- Improves readability
- Keeps core logic clean and focused

---

### 📄 `shell.h`

This header file defines **function prototypes, macros, and shared declarations**.

Responsibilities:
- Declares all functions used across multiple source files
- Defines constants and macros
- Includes required system headers
- Exposes shared interfaces between modules

This file ensures:
- Proper compilation
- Clear contracts between components
- Cleaner and safer code structure

---

### 📄 `man_1_simple_shell`

This file contains the **manual page** for the shell.

Responsibilities:
- Describes what the shell does
- Explains usage and execution modes
- Documents built-in commands
- Provides examples for users
- Follows UNIX man page conventions

The manual page allows users to access documentation using:
man ./man_1_simple_shell

---

### 📄 `AUTHORS`

This file lists the contributors to the project.

Responsibilities:
- Identifies project authors
- Credits collaboration
- Follows Holberton project requirements

---

### 📄 `README.md`

This file serves as the **main documentation** for the project.

Responsibilities:
- Explains the purpose of the project
- Documents shell behavior
- Describes architecture and execution flow
- Explains file responsibilities
- Provides compilation and usage instructions
- Clarifies limitations and design choices

The README is intended for both **technical reviewers and learners**.

---

## 🧩 Design Philosophy

Each file in this project has **a single, clear responsibility**.

This separation allows:
- Easier debugging
- Better scalability
- Cleaner logic
- Strong alignment with UNIX design principles

---


