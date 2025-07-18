#include <stdio.h>
#include <stdlib.h>
#include <string.h>

#define INITIAL_BUFFER_SIZE 128

// data structure to hold each line in a linked list
typedef struct Line {
    char *text;            
    struct Line *next; // pointer to the next line
} Line;

// function declarations for the line operations
Line *read_lines(FILE *input);
void write_reversed(Line *head, FILE *output);
void free_lines(Line *head);
char *read_line(FILE *input);

int main(int argc, char *argv[]) {
    FILE *input = stdin;
    FILE *output = stdout;
    Line *head = NULL;

    // handles if too many arguments are run
    if (argc > 3) {
        fprintf(stderr, "usage: reverse <input> <output>\n");
        exit(1);
    }

    // opens input file if its given 
    if (argc >= 2) {
        input = fopen(argv[1], "r");
        if (input == NULL) {
            fprintf(stderr, "error: cannot open file '%s'\n", argv[1]);
            exit(1);
        }
    }

    // opens output file if its provided
    if (argc == 3) {
        // checks if input and output files are the same
        if (strcmp(argv[1], argv[2]) == 0) {
            fprintf(stderr, "Input and output file must differ\n");
            exit(1);
        }

        output = fopen(argv[2], "w");
        if (output == NULL) {
            fprintf(stderr, "error: cannot open file '%s'\n", argv[2]);
            exit(1);
        }
    }

    // reads all the lines in the linked list
    head = read_lines(input);

    // extra error handling if malloc failure inside read_lines
    if (head == NULL && ferror(input)) {
        fprintf(stderr, "malloc failed\n");
        exit(1);
    }

    if (input != stdin) {
        fclose(input);
    }

    // writes the lines in reverse order to output
    write_reversed(head, output);

    // closes output file if its not stdout
    if (output != stdout) {
        fclose(output);
    }

    free_lines(head);

    return 0;
}

// reads a line of text from a file stream with dynamic memory allocation
char *read_line(FILE *input) {
    size_t buffer_size = INITIAL_BUFFER_SIZE; // buffer size earlier specified
    char *buffer = malloc(buffer_size);

    if (buffer == NULL) {
        fprintf(stderr, "malloc failed\n");
        exit(1); //terminates program if buffer is NULL
    }

    // reads the first chunk
    char *result = fgets(buffer, buffer_size, input);
    if (result == NULL) {
        free(buffer);
        return NULL; 
    }

    // keeps reading till newline is found or reached the of the file
    while (strchr(buffer, '\n') == NULL && !feof(input)) {
        buffer_size *= 2;
        char *new_buffer = realloc(buffer, buffer_size);
        if (new_buffer == NULL) {
            free(buffer); // cleans up before failing
            fprintf(stderr, "malloc failed\n");
            exit(1);
        }
        buffer = new_buffer; // switched to larger buffer 

        // raeds the next part of the line
        if (fgets(buffer + strlen(buffer), buffer_size - strlen(buffer), input) == NULL) {
            break;
        }
    }

    return buffer;
}

// reads the lines from input and stores them in a reversed linked list
Line *read_lines(FILE *input) {
    Line *head = NULL;
    char *line;

    while ((line = read_line(input)) != NULL) {
        Line *new_line = malloc(sizeof(Line));
        if (new_line == NULL) {
            fprintf(stderr, "malloc failed\n");
            exit(1);
        }

        new_line->text = line;
        new_line->next = head;
        head = new_line;
    }

    return head;
}

// prints the lines from the linked list in reverse order
void write_reversed(Line *head, FILE *output) {
    Line *current = head;
    while (current != NULL) {
        fprintf(output, "%s", current->text); 
        current = current->next;
    }
}

// frees all the memory used by the linekd list
void free_lines(Line *head) {
    Line *current = head;
    while (current != NULL) {
        Line *next = current->next;
        free(current->text);
        free(current);
        current = next;
    }
}
