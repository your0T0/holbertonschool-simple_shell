# 🐚 0x16. C – Simple Shell

> *“The shell is the bridge between the user and the operating system.”*

---

## 👥 Authors

**Manar** & **Yara**  
*Simple Shell Developers*

---

## 📋 Table of Contents

- [Project Overview](#-project-overview)
- [Learning Objectives](#-learning-objectives)
- [Shell Overview](#-shell-overview)
- [Features](#-features)
- [Built-in Commands](#-built-in-commands)
- [Execution Modes](#-execution-modes)
- [Command Parsing](#-command-parsing)
- [PATH Resolution](#-path-resolution)
- [Process Management](#-process-management)
- [Error Handling](#-error-handling)
- [Limitations](#-limitations)
- [File Structure](#-file-structure)
- [Compilation & Usage](#-compilation--usage)
- [Memory Management](#-memory-management)
- [Coding Standards](#-coding-standards)
- [Educational Purpose](#-educational-purpose)

---

## 🧭 Project Overview

This project is a simplified implementation of a UNIX command line interpreter written in **C**, developed as part of the **Holberton School Low-Level Programming curriculum**.

The shell recreates essential behaviors of `/bin/sh` by reading user input, parsing commands, resolving executable paths, creating child processes, and executing programs using system calls.

The project focuses on understanding how shells operate internally while respecting strict constraints on allowed functions and coding style.

---

## 🎯 Learning Objectives

By completing this project, we demonstrate understanding of:

- How a UNIX shell works internally  
- Process creation and synchronization  
- Program execution using system calls  
- Environment variable handling  
- PATH parsing and command lookup  
- Interactive vs non-interactive execution  
- Proper exit status handling  
- Memory-safe programming in C  
- Writing clean, structured, Betty-compliant code  

---

## 🐚 Shell Overview

A shell is a program that provides an interface between the user and the operating system.  
It interprets user commands and requests services from the kernel.

This shell supports:
- Reading commands from standard input
- Executing programs found in the system
- Handling errors and exit statuses correctly
- Running both interactively and non-interactively

---

## ⚙️ Features

### ✅ Supported Functionality

- Displays a prompt and waits for user input
- Reads input using `getline`
- Ignores empty or whitespace-only input
- Tokenizes commands using `strtok`
- Executes commands using `execve`
- Searches executables in the `PATH`
- Executes commands via absolute and relative paths
- Implements required built-in commands (`exit`, `env`)
- Handles end-of-file (`Ctrl + D`)
- Matches `/bin/sh` error output format

---

## 🔧 Built-in Commands

### `exit`
Exits the shell and returns the exit status of the last executed command.

### `env`
Prints all environment variables available to the current process.

---

## 🖥️ Execution Modes

### 🟢 Interactive Mode

When the shell is executed from a terminal:

```bash
$ ./hsh
($) /bin/ls
hsh main.c
($) exit
$
