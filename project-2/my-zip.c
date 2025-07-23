#include <stdio.h>
#include <stdlib.h>
#include <errno.h> 
#include <string.h>

void zip_file(FILE *input) {
    int current;          // current character read from file
    int previous = EOF;       // previous character (initialize to EOF)
    unsigned count = 1;        

    // processes one character at a time from the input file returns EOF when end of file is reached
    while ((current = fgetc(input)) != EOF) {
        if (current == previous) {
            count++;  
        } else {
            // when character changes then writes the previous run
            if (previous != EOF) {
                // writes in the format of 4-byte binary count + 1-byte ASCII character
                fwrite(&count, sizeof(unsigned), 1, stdout);
                fputc(previous, stdout);
            }
            // resets for the new character run 
            previous = current;
            count = 1;
        }
    }

    //writes final run if file isn't empty
    if (previous != EOF) {
        fwrite(&count, sizeof(unsigned), 1, stdout);
        fputc(previous, stdout);
    }
}

int main(int argc, char *argv[]) {
    // checks for correct invocation one or more lines should be passed via command line
    if (argc < 2) {
        fprintf(stderr, "my-zip: file1 [file2 ...]\n");
        exit(1);
    }

    // processes each input file
    for (int i = 1; i < argc; i++) {
        FILE *file = fopen(argv[i], "rb");  // opens file in binary mode
        if (!file) {
             fprintf(stderr, "my-zip: cannot open '%s': %s\n", argv[i], strerror(errno));
            exit(1);
        }

        zip_file(file);
        fclose(file);
    }

    return 0;
}