# File-Inspector-C-Analysis-and-Compilation-Utility

## Overview
File Inspector is a C-based command-line utility designed for UNIX systems that scans a specified directory for C source files (`.c`). It provides detailed file information, compiles source files using `gcc`, and manages symbolic links for smaller files. Additionally, it implements inter-process communication to filter compilation errors and warnings.

## Features
- **Directory Traversal**: Scans the specified directory for `.c` files.
- **File Information Display**: Provides details based on user-specified options:
  - `-n`: File name
  - `-u`: User ID
  - `-a`: File permissions (read/write/execute for user, group, and others)
  - `-d`: File size in bytes
  - `-c`: Hard link count
- **Symbolic Link Creation**: If a file is smaller than 100 KB, a symbolic link without the `.c` extension is created.
- **Compilation with GCC**: Uses `gcc` to compile detected `.c` files when the `-g` option is specified.
- **Process Management**:
  - Child processes handle compilation, file information retrieval, and symbolic link creation.
  - The parent process manages and retrieves exit statuses of child processes.
- **Error and Warning Filtering** (`-p` option):
  - Uses pipes and `egrep` to filter compilation warnings and errors.

## Process Flow
1. The parent process forks three child processes per `.c` file:
   - **Compilation process** (`gcc` execution)
   - **File information extraction** (options `-n, -u, -a, -d, -c`)
   - **Symbolic link creation** (if file < 100 KB)
2. If `-p` is used:
   - A pipeline filters errors/warnings.
   - The parent process computes and displays a score.
3. The parent process waits for all child processes to terminate and prints their exit statuses.

## Dependencies
- UNIX-based system
- GCC compiler (`gcc`)
- `egrep` command for error/warning filtering

## How to Compile and Run
1. Copy the source code into a `.c` file.
2. Compile the program using a C compiler:
   ```sh
   gcc -o file_inspector file_inspector.c
   ```
3. Run the executable with the desired directory and options:
   ```sh
   ./file_inspector <directory> <options>
   ```
   Example:
   ```sh
   ./file_inspector /home/user/directory -ndc
   ```
