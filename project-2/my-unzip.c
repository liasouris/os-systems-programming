#include <stdio.h>
#include <stdlib.h>
#include <errno.h>  
#include <string.h>

void unzip_file(FILE *input) {
    unsigned count;    // stores the run length
    char character;    // stores the character to repeat

    // reads the 4-byte count + 1-byte char)
    while (fread(&count, sizeof(unsigned), 1, input) == 1) {
        if (fread(&character, sizeof(char), 1, input) != 1) {
            fprintf(stderr, "my-unzip: invalid compressed file format\n");
            exit(1);
        }

        // prints character to stdout count amount of times
        for (unsigned i = 0; i < count; i++) {
            putchar(character);
        }
    }
}

int main(int argc, char *argv[]) {
    // checks for correct invocation one or more lines should be passed via command line
    if (argc < 2) {
        fprintf(stderr, "my-unzip: file1 [file2 ...]\n");
        exit(1);
    }
    // processes each compressed file
    for (int i = 1; i < argc; i++) {
        FILE *file = fopen(argv[i], "rb");  // opens in binary mode
        if (!file) {
            fprintf(stderr, "my-unzip: cannot open '%s': %s\n", argv[i], strerror(errno));
            exit(1);
        }

        unzip_file(file);
        fclose(file);
    }

    return 0;
}