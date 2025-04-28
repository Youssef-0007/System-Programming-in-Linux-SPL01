
int echo_main(int argc, char *argv[]) {
    // Write your code here
    // Do not write a main() function. Instead, deal with echo_main() as the main function of your program.
    for(int i = 1; i < argc; ++i) {
        write(STDOUT_FILENO, argv[i], strlen(argv[i]));
        if(i < argc - 1)
            write(STDOUT_FILENO, " ", 1);
    }
    write(STDOUT_FILENO, "\n",1);
    return 0;
}