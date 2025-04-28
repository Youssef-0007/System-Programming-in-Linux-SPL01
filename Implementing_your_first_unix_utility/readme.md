# My Unix Utilities

This repository contains implementations of basic Unix utilities created as part of the **System Programming in Linux** course on Udemy. These utilities are designed to showcase system programming concepts in a simplified manner. Each utility replicates fundamental functionality of its Unix counterpart. Below is a detailed overview of the implemented utilities.

---

## Utilities Overview

### 1. **pwd** - Print Working Directory
Prints the current working directory to the standard output.

- **Description:** Utilizes the `getcwd()` function to retrieve and display the absolute pathname of the current working directory.
- **Usage Example:**
  ```bash
  '''$ ./pwd
  /home/user/projects
'''
  ### 2. **echo** - Print User Input
  Prints a user-provided string to the standard output.
  - **Description**: Mimics the Unix echo command by taking input and directly outputting it to the standard output.
  - **Usage Example:**
$ ./echo "Hello, World!"
Hello, World!

### 3. cp - Copy a File
Creates a copy of a specified source file at a given destination.
- **Description:** This utility functions like a simplified version of the Unix cp command. It supports copying a single file to a defined destination.
- **Usage Example:**
$ ./cp file.txt /tmp/file_copy.txt

### 4. mv - Move a File
Moves a file from a source location to a destination, with optional renaming
- **Description:** Mimics the Unix mv command by supporting basic file relocation and renaming.
- **Usage Examples:**
$ ./mv /tmp/file.txt /tmp/new_name.txt
$ ./mv /tmp/file.txt /home/reda/new_name.txt
$ ./mv /tmp/file.txt /home/reda/file.txt


## Compilation Instructions
To compile the utilities, use the gcc compiler. Below are sample commands for compiling each utility:
gcc -o pwd pwd.c
gcc -o echo echo.c
gcc -o cp cp.c
gcc -o mv mv.c

## Running the Utilities
Each utility can be executed as follows:
- pwd: ./pwd
- echo: ./echo "Your message here"
- cp: ./cp source_file destination_file
- mv: ./mv source_file destination_file
















