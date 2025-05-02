#include <unistd.h>
#include <string.h>
#include <stdlib.h>

// Define file descriptors
#define STDIN_FD  0
#define STDOUT_FD 1
#define MAX_LINE 1024

char *prompt = "Femto shell prompt > ";  // Matching the example prompt exactly


// Function to write string to stdout using write syscall
void write_stdout(const char *str) {
    write(STDOUT_FD, str, strlen(str));
}

int read_stdin(char** result) {
    char buffer[MAX_LINE];
    ssize_t total_size = 0;
    ssize_t bytes_read;
    char* full_input = NULL;

    while (1) {
        // Read input using read syscall
        bytes_read = read(STDIN_FD, buffer, MAX_LINE - 1);

        // Check for read errors
        if (bytes_read < 0) {
            perror("Error reading from stdin");
            free(full_input);
            return -1;
        }

        // End of input
        if (bytes_read == 0) {
            break;
        }

        // Resize the buffer
        char* temp = (char*) realloc(full_input, total_size + bytes_read + 1);
        if (!temp) {
            perror("Memory allocation failed");
            free(full_input);
            return -1;
        }

        full_input = temp;

        // Copy buffer to the extended memory
        memcpy(full_input + total_size, buffer, bytes_read);
        total_size += bytes_read;
    }

    // Null-terminate the string
    if (full_input) {
        full_input[total_size] = '\0';
    }

    *result = full_input;
    return total_size;
}

// Function to compare strings (since we can't use strcmp)
int str_compare(const char *s1, const char *s2) {
    while (*s1 && (*s1 == *s2)) {
        s1++;
        s2++;
    }
    return *s1 - *s2;
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


// Function to check if a string is empty or whitespace only
int is_empty_or_whitespace(const char *str) {
    while (*str) {
        if (*str != ' ' && *str != '\t' && *str != '\n' && *str != '\r') {
        return 0;  // Found non-whitespace character
        }
            str++;
        }
    return 1;  // All characters are whitespace or string is empty
}

int process_command(const char* cmd_line) {
    int status = 0;
    
    // Check if input is empty or just whitespace
    if (is_empty_or_whitespace(cmd_line)) {
        // Display prompt using write syscall
        write_stdout(prompt);
        write_stdout(prompt);    
        return 0;
    }
    
    // Handle "exit" command
    if (str_compare(cmd_line, "exit") == 0) {
        write_stdout("Good Bye\n");
        // Keep the last command status when exiting
        return -1;
    }
    
    // Handle "echo" command
    if (starts_with(cmd_line, "echo ")) {
        // Extract text after "echo " and print it
        const char *text = cmd_line + 5;  // 5 is the length of "echo "
        write_stdout(text);
        write_stdout("\n");
        status = 0;  // Success
    }
    // Handle bare "echo" command with no arguments
    else if (str_compare(cmd_line, "echo") == 0) {
        write_stdout("\n");
        status = 0;  // Success
    }
    // Handle backslash character
    else if (str_compare(cmd_line, "\\") == 0) {
        write_stdout("\n");
        status = 0;  // Success
    }
    // Invalid command
    else {
        write_stdout("Invalid command\n");
        status = 1;  // Command failed
    }
    
    return status;
}

// Since we can't implement main(), let's use the provided function
int femtoshell_main(int argc, char *argv[]) {
    char* input_buffer = NULL;  // Buffer for user input
    char* cmd_buffer = NULL;  // Buffer for each command
    int bytes_read;
    int running = 1;
    int last_command_status = 0;  // Track status of last command
    
    while (running) {
        // Display prompt using write syscall
        write_stdout(prompt);
        
        /*// Read input using read syscall
        bytes_read = read(STDIN_FD, input_buffer, sizeof(input_buffer) - 1);
        
        // Check for read errors
        if (bytes_read < 0) {
            last_command_status = 1;  // Error condition
            break;
        }
        
        // Handle EOF (Ctrl+D)
        if (bytes_read == 0) {
            // For EOF, keep the last command status instead of always returning success
            break;
        }

        // Null-terminate the input
        input_buffer[bytes_read] = '\0';*/
        
        /*// Remove trailing newline if present
        if (bytes_read > 0 && input_buffer[bytes_read - 1] == '\n') {
            input_buffer[bytes_read - 1] = '\0';
            bytes_read--;
        }
        
        // Handle empty input (just pressing Enter)
        if (bytes_read == 0) {
            // Empty command doesn't change the last status
            continue;
        }*/
        
        bytes_read = read_stdin(&input_buffer);
        
        // Check for read errors
        if (bytes_read < 0) {
            last_command_status = 1;  // Error condition
            break;
        }
        
        // Handle EOF (Ctrl+D)
        if (bytes_read == 0) {
            // For EOF, keep the last command status instead of always returning success
            break;
        }
        
        int cmd_start = 0;
        int i = 0;
        
        while (i < bytes_read) {
            // Find the next newline character
            while (i < bytes_read && input_buffer[i] != '\n') {
                i++;
            }
            /*char str[20];
            sprintf(str, "%d", i);
            write_stdout(str);*/
            // Found a newline or end of Buffer
            if(i >= bytes_read || input_buffer[i] == '\n') {
                // Save the current command
                int cmd_length = i - cmd_start;
                if(cmd_length > 0) {
                    cmd_buffer = (char*) malloc(sizeof(char) * cmd_length);
                    int j;
                    for(j = 0; j < cmd_length; ++j) {
                        cmd_buffer[j] = input_buffer[cmd_start + j];
                    }
                    // add Null-terminate
                    cmd_buffer[j] = '\0';
                    
                    int status = process_command(cmd_buffer);
                    
                    if(status == -1) {
                        running = 0;
                        last_command_status = 0;
                        break;
                    }
                    
                    last_command_status = status;
                }
                
                // Move past the newline character
                if(i < bytes_read) {
                    ++i;
                }
                
                // Next command starts after newline 
                cmd_start = i;
            }
            
            if(i < bytes_read) {
                // Display prompt using write syscall
                write_stdout(prompt);
            }
        }
    }
    
    // Return the status of the last command
    return last_command_status;
        
}