#include <unistd.h>
#include <string.h>
#include <stdlib.h>
#include <sys/wait.h>
#include <sys/types.h>
#include <stdio.h>  // For perror and errno
#include <errno.h>  // For errno

// Define file descriptors
#define STDIN_FD        0
#define STDOUT_FD       1
#define MAX_LINE        1024
#define PATH_MAX_LEN    1024

char *prompt = "Pico shell prompt > ";  // Matching the example prompt exactly

// Function to write string to stdout using write syscall
void write_stdout(const char *str) {
    write(STDOUT_FD, str, strlen(str));
}

int read_stdin(char** result) {
    char buffer[MAX_LINE];
    ssize_t bytes_read;
    char* line = (char*)malloc(MAX_LINE);
    
    if (!line) {
        perror("Memory allocation failed");
        return -1;
    }
    
    int index = 0;
    int capacity = MAX_LINE;
    
    while ((bytes_read = read(STDIN_FD, buffer, MAX_LINE - 1)) > 0) {
        // Make sure we have enough space
        if (index + bytes_read >= capacity) {
            capacity *= 2;
            char* temp = (char*)realloc(line, capacity);
            if (!temp) {
                perror("Memory allocation failed");
                free(line);
                return -1;
            }
            line = temp;
        }
        
        // Copy the bytes
        memcpy(line + index, buffer, bytes_read);
        index += bytes_read;
        
        // Check if we reached the end of input
        if (bytes_read < MAX_LINE - 1 || buffer[bytes_read - 1] == '\n') {
            break;
        }
    }
    
    if (bytes_read < 0) {
        perror("Error reading from stdin");
        free(line);
        return -1;
    }
    
    // Null-terminate the result
    if (index >= capacity) {
        char* temp = (char*)realloc(line, index + 1);
        if (!temp) {
            perror("Memory allocation failed");
            free(line);
            return -1;
        }
        line = temp;
    }
    line[index] = '\0';
    
    *result = line;
    return index;
}

// Check if string starts with prefix
int starts_with(const char *str, const char *prefix) {
    while (*prefix) {
        if (*prefix != *str) {
            return 0;
        }
        prefix++;
        str++;
    }
    return 1;
}

// Function to compare strings
int str_compare(const char *s1, const char *s2) {
    while (*s1 && (*s1 == *s2)) {
        s1++;
        s2++;
    }
    return *s1 - *s2;
}

// Function to check if a string is empty or whitespace only
int is_empty_or_whitespace(const char *str) {
    //write_stdout("check empty space or newline\n");
    while (*str) {
        if (*str != ' ' && *str != '\t' && *str != '\n' && *str != '\r') {
            return 0;  // Found non-whitespace character
        }
        str++;
    }
    return 1;  // All characters are whitespace or string is empty
}

char** split_by_newline(char* input_buffer, int length, int* cmd_count) {
    char** commands = NULL;
    int cmd_start = 0;
    int count = 0;
    
    for (int i = 0; i <= length; ++i) {
        // Found a newline or end of buffer
        if (i == length || input_buffer[i] == '\n') {
            // Save the current command
            int cmd_length = i - cmd_start;
            if (cmd_length > 0) {
                char* cmd = (char*)malloc(sizeof(char) * (cmd_length + 1));
                if (!cmd) {
                    perror("Memory allocation failed");
                    // Free all previous commands
                    for (int j = 0; j < count; j++) {
                        free(commands[j]);
                    }
                    free(commands);
                    return NULL;
                }
                
                memcpy(cmd, input_buffer + cmd_start, cmd_length);
                cmd[cmd_length] = '\0';
                
                // Reallocate commands array
                char** temp = (char**)realloc(commands, sizeof(char*) * (count + 2));
                if (!temp) {
                    perror("Memory allocation failed");
                    free(cmd);
                    // Free all previous commands
                    for (int j = 0; j < count; j++) {
                        free(commands[j]);
                    }
                    free(commands);
                    return NULL;
                }
                
                commands = temp;
                commands[count++] = cmd;
                commands[count] = NULL;
            }
            
            cmd_start = i + 1;
        }
    }
    //Print("%d\n",count)
    *cmd_count = count;
    return commands;
}

char** parse_cmd(char* command, int* argc_out) {
    char** argv = NULL;
    int count = 0;
    
    int i = 0;
    int len = strlen(command);
    
    while (i < len) {
        // Skip leading spaces
        while (i < len && command[i] == ' ') i++;
        
        if (i >= len) break;
        
        // Find the end of this argument
        int start = i;
        while (i < len && command[i] != ' ') i++;
        
        int token_len = i - start;
        if (token_len > 0) {
            char* arg = (char*)malloc(token_len + 1);
            if (!arg) {
                perror("Memory allocation failed");
                
                // Clean up previously allocated args
                for (int j = 0; j < count; j++) {
                    free(argv[j]);
                }
                free(argv);
                return NULL;
            }
            
            memcpy(arg, command + start, token_len);
            arg[token_len] = '\0';
            
            // Add to argv
            char** temp = (char**)realloc(argv, sizeof(char*) * (count + 2));
            if (!temp) {
                perror("Memory allocation failed");
                free(arg);
                
                // Clean up previously allocated args
                for (int j = 0; j < count; j++) {
                    free(argv[j]);
                }
                free(argv);
                return NULL;
            }
            
            argv = temp;
            argv[count++] = arg;
            argv[count] = NULL;
        }
    }
    
    *argc_out = count;
    return argv;
}

int process_command(char* cmd) {
    if (!cmd || is_empty_or_whitespace(cmd)) {
        write_stdout(prompt);
        write_stdout(prompt);
        return 0;
    }
    
    int argc = 0;
    char** argv = parse_cmd(cmd, &argc);
    
    if (!argv || argc == 0) {
        return 0;
    }
    
    int result = 0;
    
    // Handle "exit" command
    if (str_compare(argv[0], "exit") == 0) {
        write_stdout("Good Bye\n");
        result = -1;  // Signal to exit
    }
    
    // Handle "echo" command
    else if (str_compare(argv[0], "echo") == 0) {
        if (argc == 1) {
            write_stdout("\n");
        } else {
            for (int i = 1; i < argc; ++i) {
                write_stdout(argv[i]);
                if (i != argc - 1)
                    write_stdout(" ");
            }
            write_stdout("\n");
        }
    }
    
    // Handle "pwd" command
    else if (str_compare(argv[0], "pwd") == 0) {
        char cwd[PATH_MAX_LEN];
        if (getcwd(cwd, sizeof(cwd)) != NULL) {
            write_stdout(cwd);
            write_stdout("\n");
        } else {
            perror("pwd failed");
            result = 1;
        }
    }
    
    // Handle "cd" command
    else if (str_compare(argv[0], "cd") == 0) {
        if (argc < 2) {
            write_stdout("cd: missing argument\n");
            result = 1;
        } else if (chdir(argv[1]) != 0) {
            write_stdout("cd: ");
            write_stdout(argv[1]);
            write_stdout(": ");
            char* error = strerror(errno);
            write_stdout(error);
            write_stdout("\n");
            result = 1;
        }
    }
    
    // External command
    else {
        pid_t pid = fork();
        
        if (pid == 0) {
            // Child process
            execvp(argv[0], argv);
            // If execvp returns, it means an error occurred
            write_stdout(argv[0]);
            write_stdout(": command not found\n");
            _exit(127);  // Exit with error status
        } else if (pid > 0) {
            // Parent process
            int status;
            waitpid(pid, &status, 0);
            if (WIFEXITED(status)) {
                result = WEXITSTATUS(status);
            } else {
                result = 1;  // Something went wrong
            }
        } else {
            // Fork failed
            perror("fork failed");
            result = 1;
        }
    }
    
    // Clean up
    for (int i = 0; i < argc; i++) {
        free(argv[i]);
    }
    free(argv);
    
    return result;
}

int picoshell_main(int argc, char *argv[]) {
    int running = 1;
    int last_command_status = 0;
    
    while (running) {
        // Display prompt
        write_stdout(prompt);
        
        // Read input
        char* input_buffer = NULL;
        int bytes_read = read_stdin(&input_buffer);
        
        if (bytes_read <= 0) {
            // EOF or error
            if (bytes_read < 0) {
                perror("Error reading input");
            }
            free(input_buffer);
            break;
        }
        
        // Split input into commands
        int command_count = 0;
        char** commands = split_by_newline(input_buffer, bytes_read, &command_count);
        
        if (commands == NULL || command_count == 0) {
            // No commands to process
            free(input_buffer);
            write_stdout(prompt);
            write_stdout(prompt);
            continue;
        }
        
        // Process each command
        for (int i = 0; i < command_count; i++) {
            int status = process_command(commands[i]);
            
            if (status == -1) {
                // Exit command
                running = 0;
                last_command_status = 0;
                break;
            }
            
            last_command_status = status;
            
            // Print prompt after each command except the last one
            if (running && i < command_count - 1) {
                write_stdout(prompt);
            } else if(command_count > 1 && i == command_count - 1 && starts_with(commands[i], "echo ")) {
                write_stdout(prompt);
            }
            
        }
        
        /*if (running)
            write_stdout(prompt);
        */        
        // Clean up
        for (int i = 0; i < command_count; i++) {
            free(commands[i]);
        }
        free(commands);
        free(input_buffer);
    }
    
    return last_command_status;
}