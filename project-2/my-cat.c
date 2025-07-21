#include <stdio.h>
#include <stdlib.h>

int main(int argc, char *argv[]) {
    if (argc < 2) { // exits and returns 0 if no files are specified
        return 0;  
    }

    for (int i = 1; i < argc; i++) { // loops through each filename skipping the first program name
        FILE *fp = fopen(argv[i], "r"); // opens the file in read mode
        if (fp == NULL) {  // if no file is found or failure to open file returns error message
            fprintf(stderr, "my-cat: cannot open file\n");
            exit(1);
        }

        char buffer[1024];
        while (fgets(buffer, sizeof(buffer), fp) != NULL) {
            // fgets reads until new line or the buffer is full
            printf("%s", buffer);
        }

        fclose(fp); // closes the opened files
    }

    return 0;
}