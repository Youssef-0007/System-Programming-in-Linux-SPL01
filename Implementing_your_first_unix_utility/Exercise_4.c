#include <stdio.h>
#include <stdlib.h>
#include <string.h>
#include <unistd.h>
#include <fcntl.h>
#include <errno.h>
#include <sys/stat.h>

#define BUFF_SIZE 1024

void fatal(const char *msg) {
    perror(msg);
    exit(EXIT_FAILURE);
}

int mv_main(int argc, char *argv[]) {
    if (argc != 3 || strcmp(argv[1], "--help") == 0) {
        fprintf(stderr, "Usage: mv source_file target_file\n");
        exit(EXIT_FAILURE);
    }

    if (rename(argv[1], argv[2]) == 0) {
        return 0; // rename worked
    }

    // Check if rename failed because of EXDEV (cross-filesystem move)
    if (errno != EXDEV) {
        fatal("rename");
    }

    // fallback to manual copy + unlink
    int src_fd = open(argv[1], O_RDONLY);
    if (src_fd == -1) {
        fatal("open source");
    }

    int dest_fd = open(argv[2], O_WRONLY | O_CREAT | O_TRUNC, 0666);
    if (dest_fd == -1) {
        close(src_fd);
        fatal("open destination");
    }

    char buf[BUFF_SIZE];
    ssize_t numRead;
    while ((numRead = read(src_fd, buf, BUFF_SIZE)) > 0) {
        if (write(dest_fd, buf, numRead) != numRead) {
            close(src_fd); close(dest_fd);
            fatal("write");
        }
    }
    if (numRead == -1) {
        close(src_fd); close(dest_fd);
        fatal("read");
    }

    close(src_fd);
    close(dest_fd);

    if (unlink(argv[1]) == -1) {
        fatal("unlink source");
    }

    return 0;
}
