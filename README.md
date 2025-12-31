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


