#include <stdio.h>
#include <stdlib.h>
#include <string.h>
#include <unistd.h>
#include <sys/wait.h>
#include <sys/types.h>
#include <errno.h> 

#define MAX_LINE 1024 // max command line length 1024 char
#define MAX_ARGS 64 // max arguments 64 tokens

void execute_command(char **args) {
    pid_t pid = fork(); // creates new process
    
    if (pid == 0) { // child process
        execv(args[0], args);
        // if execv fails then returns error message
        fprintf(stderr, "An error has occurred\n");
        exit(1);
    } else if (pid > 0) { // parent process
        wait(NULL); 
    } else {
        fprintf(stderr, "An error has occurred\n");
    }
}
// program can run in the two modes interactive or batch mode if a file is given in argument
int main(int argc, char *argv[]) {
    char line[MAX_LINE];
    char *args[MAX_ARGS];
    FILE *input = stdin;
    
    // checks for batch mode
    if (argc > 2) { // too many arguments
    fprintf(stderr, "An error has occurred\n");
    exit(1);
} else if (argc == 2) {
    input = fopen(argv[1], "r"); // open file in read mode
    if (input == NULL) {
        fprintf(stderr, "cannot open '%s': %s\n", argv[1], strerror(errno));
        exit(1);
    }
}
    
    //main shell loop in interactive mode shows a prompt
    while (1) {
        if (input == stdin) {
            printf("wish> ");
            fflush(stdout);
        }
        
        if (fgets(line, MAX_LINE, input) == NULL) {
            if (feof(input)) {
                exit(0); // end of file exit
            } else {
                fprintf(stderr, "An error has occurred\n");
                continue;
            }
        }
        
        // clean up removes new line 
        line[strcspn(line, "\n")] = '\0';
        
        // tokenizes input
        char *token = strtok(line, " ");
        int i = 0;
        while (token != NULL && i < MAX_ARGS - 1) {
            args[i++] = token;
            token = strtok(NULL, " ");
        }
        args[i] = NULL;
        
        // empty command handling
        if (args[0] == NULL) {
            continue;
        }
        
        // build in commands
        if (strcmp(args[0], "exit") == 0) {
            if (args[1] != NULL) {
                fprintf(stderr, "An error has occurred\n");
                continue;
            }
            exit(0);
        } else if (strcmp(args[0], "cd") == 0) {
            if (args[1] == NULL || args[2] != NULL) {
                fprintf(stderr, "An error has occurred\n");
                continue;
            }
            if (chdir(args[1]) != 0) {
                fprintf(stderr, "An error has occurred\n");
            }
            continue;
        } else if (strcmp(args[0], "path") == 0) {
            // path implementation **to be edited**
            printf("Path command received\n");
            continue;
        }
        
        // if command is not built-in, it assumes the executable is in /bin/
        // prepends /bin/ to the command and then tries to then run it
        char full_path[256];
        snprintf(full_path, sizeof(full_path), "/bin/%s", args[0]);
        args[0] = full_path;
        
        execute_command(args);
    }
    
    return 0;
}