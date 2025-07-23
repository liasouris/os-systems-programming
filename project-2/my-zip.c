#include <stdio.h>

void zip_file(FILE *input) {
    int current, previous = EOF;
    unsigned count = 1;
    
    // reads one character at a time from the input file returns EOF when end of file is reacehd
    while ((current = fgetc(input)) != EOF) {
        if (current == previous) {
            count++;
        } else {
            if (previous != EOF) {
                // Format: 4-byte binary count + 1-byte ASCII character
                fwrite(&count, sizeof(unsigned), 1, stdout);
                putchar(previous);
            }

            // resets for new character run 
            previous = current;
            count = 1;
        }
    }
    
    // after EOF, writes the final run if there was any data
    if (previous != EOF) {
        fwrite(&count, sizeof(unsigned), 1, stdout);
        putchar(previous);
    }
}

int main(int argc, char *argv[]) {
    for (int i = 1; i < argc; i++) {
        FILE *file = fopen(argv[i], "r");
        zip_file(file);
        fclose(file);
    }
    return 0;
}