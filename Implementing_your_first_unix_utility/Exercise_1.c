#include<limits.h>
#include<unistd.h>

int pwd_main() {
    // Write your code here
    // Do not write a main() function. Instead, deal with pwd_main() as the main function of your program.
    char buffer[PATH_MAX] = {};
    
    if(getcwd(buffer, PATH_MAX) == NULL) {
        perror("pwd");
        return 1;
    } else {
        write(STDOUT_FILENO, buffer, strlen(buffer));
        write(STDOUT_FILENO, "\n", 1);
        return 0;
    }
}