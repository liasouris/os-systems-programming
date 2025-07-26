#include <stdio.h>
#include <stdlib.h>
#include <string.h>
#include <unistd.h>
#include <sys/wait.h>
#include <fcntl.h>

#define MAX_LINE 1024 // max command line length 1024 char
#define MAX_ARGS 64 // max arguments per command
#define MAX_CMDS 10 //max parallel commands
#define MAX_PATHS 10 // max diretories in serach path

char *paths[MAX_PATHS] = {"/bin"};// default search path
int path_count = 1; // current path count

void execute_commands(char **args, int redirect, char *file) {
    pid_t pid = fork(); // creates child process
    if (pid == 0) {
        // handles the output redirection if its specified
        if (redirect) {
            int fd = open(file, O_WRONLY|O_CREAT|O_TRUNC, 0644);
            //redirects both standard output and standard error to the specified file
            dup2(fd, STDOUT_FILENO); 
            dup2(fd, STDERR_FILENO);
            close(fd);
        }
        // tries executing command in each path directory
        for (int i = 0; i < path_count; i++) {
            char full_path[256];
            snprintf(full_path, sizeof(full_path), "%s/%s", paths[i], args[0]);
            execv(full_path, args);
        }
        // if execv return then command wasnt found
        fprintf(stderr, "An error has occurred\n");
        exit(1);
    }
}

void parallel_commands(char *line) {
    char *commands[MAX_CMDS][MAX_ARGS]; // array to hold the parsed argument for each command
    int cmd_count = 0; // parsed commands count
    char *saveptr;
    
    // first splits the input line using the "&" as seprator to get each command
    char *cmd_str = strtok_r(line, "&", &saveptr);
    while (cmd_str && cmd_count < MAX_CMDS) {
        // skips the white spaces
        while (*cmd_str == ' ' || *cmd_str == '\t') cmd_str++;
        //if command is empty show error
        if (*cmd_str == '\0') {
            fprintf(stderr, "An error has occurred\n");
            return;
        }

        // splits the comamnd into arguments using space as separator
        int arg_count = 0;
        char *token = strtok(cmd_str, " \t");
        while (token && arg_count < MAX_ARGS-1) {
            commands[cmd_count][arg_count++] = token;
            token = strtok(NULL, " \t");
        }
        
        // no valid argument found shows error
        if (arg_count == 0) {
            fprintf(stderr, "An error has occurred\n");
            return;
        }
         // adds NULL at the end of the arguments list so execv knows where the list ends
        commands[cmd_count][arg_count] = NULL;
        cmd_count++;
        //moves to the next command in the input line
        cmd_str = strtok_r(NULL, "&", &saveptr);
    }

    pid_t pids[MAX_CMDS]; // array keeps track of the child pids
    int pid_count = 0;
    // forks a new child process for each parsed comamnd
    for (int i = 0; i < cmd_count; i++) {
        if (!commands[i][0]) continue; // skipped if command empty
        
        pids[pid_count++] = fork(); // create new child process

        if (pids[pid_count-1] == 0) {
            execute_commands(commands[i], 0, NULL); // executes without redirection
            exit(0);
        }
    }

    // waits for all child processes to be complete
    for (int i = 0; i < pid_count; i++) {
        waitpid(pids[i], NULL, 0);
    }
}

int main(int argc, char *argv[]) {
    char line[MAX_LINE];
    // sets input source (stdin or batch file)
    FILE *input = argc == 2 ? fopen(argv[1], "r") : stdin;
    // checks for invalid arguments
    if (argc > 2 || (argc == 2 && !input)) {
        fprintf(stderr, "An error has occurred\n");
        return 1;
    }
    // main shell loop
    while (1) {
        // prints the prompt wish> in interactive mode
        if (input == stdin) printf("wish> "), fflush(stdout);

        if (!fgets(line, MAX_LINE, input)) break;
        
        // removes new line and skips empty ones
        line[strcspn(line, "\n")] = 0;
        if (!line[0]) continue;

        // checks for parallel commands
        if (strchr(line, '&')) {
            parallel_commands(line);
            continue;
        }

        // handles single commands
        char *args[MAX_ARGS], *token = strtok(line, " \t");
        int i = 0, redirect = 0;
        char *file = NULL;
        
        // tokenize command and argument
        while (token && i < MAX_ARGS-1) {
            if (!strcmp(token, ">")) {
                if (!(file = strtok(NULL, " \t")) || strtok(NULL, " \t")) {
                    fprintf(stderr, "An error has occurred\n");
                    goto next;
                }
                redirect = 1;
                break;
            }
            args[i++] = token;
            token = strtok(NULL, " \t");
        }
        args[i] = NULL; // NULL terminate arguments

        if (!args[0]) continue;

        // handles all the built-in commands
        if (!strcmp(args[0], "exit")) {
            if (args[1]) fprintf(stderr, "An error has occurred\n");
            else return 0;
        } 
        else if (!strcmp(args[0], "cd")) {
            if (!args[1] || args[2] || chdir(args[1])) 
                fprintf(stderr, "An error has occurred\n");
        } 
        else if (!strcmp(args[0], "path")) {
            path_count = i-1; // updates the path count
            for (int j = 0; j < path_count; j++)
                paths[j] = args[j+1];
        }
        else {
            // executes external command
            if (!path_count) fprintf(stderr, "An error has occurred\n");
            else execute_commands(args, redirect, file);
            if (redirect) wait(NULL);
        }
        next:;
    }
    return 0;
}