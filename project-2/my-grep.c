#include <stdio.h>
#include <stdlib.h>
#include <string.h>

#define INITIAL_BUFFER_SIZE 128

void search_file(FILE *fp, const char *searchterm) {
    char *buffer = NULL;
    size_t line_length;
    size_t buffer_size = 0;
    
    //Reads a line from the file stream getline automatically reallocates memory if line is longer than buffer
    while ((line_length = getline(&buffer, &buffer_size, fp)) != -1) {
        //check if search term exits in the line (case-sensitive)
        if (strstr(buffer, searchterm) != NULL) {
            printf("%s", buffer);
        }
    }
    
    free(buffer); //frees the allocated memory
}

int main(int argc, char *argv[]) {
    // checks if atleast one argument is provided if no command line arguments given exits with 1 
    if (argc < 2) {
        fprintf(stderr, "my-grep: searchterm [file ...]\n");
        exit(1);
    }

    const char *searchterm = argv[1]; // gets search term from arguments

    // only search term provided
    if (argc == 2) {
        search_file(stdin, searchterm);
    }

    // opens files in read mode if fails prints error message and exits with 1
    else {
        for (int i = 2; i < argc; i++) {
            FILE *fp = fopen(argv[i], "r");
            if (fp == NULL) {
                fprintf(stderr, "my-grep: cannot open file\n");
                exit(1);
            }
            
            search_file(fp, searchterm); 
            fclose(fp);
        }
    }

    return 0; 
}