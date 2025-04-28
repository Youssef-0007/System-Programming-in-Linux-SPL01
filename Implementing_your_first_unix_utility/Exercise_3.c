#include<unistd.h>
#include<stdio.h>
#include<stdlib.h>
#include<fcntl.h>

#ifndef BUFF_SIZE
#define BUFF_SIZE 1024
#endif

void usageErr(const char* msg) {
    fprintf(stderr, "%s\n", msg);
    exit(1);
}

void fatal(const char* msg) {
    fprintf(stderr, "%s\n", msg);
    exit(1);
}

int cp_main(int argc, char *argv[]) {
    // Write your code here
    // Do not write a main() function. Instead, deal with cp_main() as the main function of your program.
    int src_fd, dest_fd, openFlags;
    mode_t filePerms;
    ssize_t numRead;
    char buffer[BUFF_SIZE];
    
    if(argc != 3 || strcmp(argv[1], "--help") == 0)
        usageErr("Usage: cp source-file destination-file\n");
    
    src_fd = open(argv[1], O_RDONLY);
    if(src_fd == -1){
        perror("opening source file %s");
        exit(EXIT_FAILURE);
    }
    openFlags = O_CREAT | O_WRONLY | O_TRUNC;
    filePerms = S_IRUSR | S_IWUSR | S_IRGRP | S_IWGRP | S_IROTH | S_IWOTH;
    
    dest_fd = open(argv[2], openFlags, filePerms);
    if(dest_fd == -1) {
        perror("opening destination file %s");
        exit(EXIT_FAILURE);
    }
    /* Transfer data until we encounter end of input or an error */
    while((numRead = read(src_fd, buffer, BUFF_SIZE)) > 0) {
        if(write(dest_fd, buffer, numRead) != numRead)
            fatal("Couldn't write whole buffer");
    }
    if(numRead == -1)
        perror("read");
    
    if(close(src_fd) == -1)
        perror("close source file!");
    if(close(dest_fd) == -1)
        perror("close destination file!");
    exit(EXIT_SUCCESS);
}