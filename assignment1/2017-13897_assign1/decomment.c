 /*
    * Name: James Kyoung Ho Kim
    * Assignment: Assignment #1 - Decomment Program
    * File: decomment.c
    *
    * This program processes a C source file and removes all single-line 
    * and multi-line comments, preserving the rest of the content.
 */

#include <stdio.h>
#include <stdlib.h>

typedef enum {
    DEFAULT,
    COMMENT_START,
    SLC,
    MLC,
    MLC_END,
    IN_STRING,
    IN_CHAR
} State;

// Function prototypes
void handle_default(char ch, State *state, FILE *output);
void handle_comment_start(char ch, State *state, FILE *output);
void handle_slc(char ch, State *state, FILE *output);
void handle_mlc(char ch, State *state, FILE *output);
void handle_mlc_end(char ch, State *state, FILE *output);
void handle_in_string(char ch, State *state, FILE *output);
void handle_in_char(char ch, State *state, FILE *output);

int main(void) {
    int ich;
    char ch;
    int line = 1;
    State currentState = DEFAULT;
    FILE *output = stdout;

    while ((ich = getchar()) != EOF) {
        ch = (char) ich;

        if (ch == '\n') {
            line++;  // Increment line number when a newline is encountered
        }

        switch(currentState) {
            case DEFAULT:
                handle_default(ch, &currentState, output);
                break;
            case COMMENT_START:
                handle_comment_start(ch, &currentState, output);
                break;
            case SLC:
                handle_slc(ch, &currentState, output);
                break;
            case MLC:
                handle_mlc(ch, &currentState, output);
                break;
            case MLC_END:
                handle_mlc_end(ch, &currentState, output);
                break;
            case IN_STRING:
                handle_in_string(ch, &currentState, output);
                break;
            case IN_CHAR:
                handle_in_char(ch, &currentState, output);
                break;
        }
    }

    if (currentState == SLC || currentState == MLC || currentState == MLC_END) {
      fprintf(stderr, "Error: line %d: unterminated comment\n", line);
      return EXIT_FAILURE;
    }

    return EXIT_SUCCESS;
}

// Handle DEFAULT state
void handle_default(char ch, State *state, FILE *output) {
    if (ch == '/') {
        *state = COMMENT_START;
    } else if (ch == '"') {
        *state = IN_STRING;
        fprintf(output, "\"");
    } else if (ch == '\'') {
        *state = IN_CHAR;
        fprintf(output, "\'");
    } else {
        fprintf(output, "%c", ch);
    }
}

// Handle COMMENT_START state
void handle_comment_start(char ch, State *state, FILE *output) {
    if (ch == '/') {
        *state = SLC;
    } else if (ch == '*') {
        fprintf(output, " "); // Print space to show comment has been removed
        *state = MLC;
    } else {
        fprintf(output, "/%c", ch); // Restore '/' if not part of a comment
        *state = DEFAULT;
    }
}

// Handle SLC state
void handle_slc(char ch, State *state, FILE *output) {
    if (ch == '\n') {
        fprintf(output, " \n"); // Print space to show comment has been removed and preserve line number
        *state = DEFAULT;
    }
}

// Handle MLC state
void handle_mlc(char ch, State *state, FILE *output) {
    if (ch == '\n') {
        fprintf(output, "\n"); // Print newline for each newline within the comment
    } else if (ch == '*') {
        *state = MLC_END;
    }
}

// Handle MLC_END state
void handle_mlc_end(char ch, State *state, FILE *output) {
    if (ch == '/') {
        *state = DEFAULT;
    } else if (ch == '\n') {
        fprintf(output, "\n"); // Preserve newline inside multi-line comments
        *state = MLC;
    } else if (ch == '*') {
        *state = MLC_END;
    } else {
        *state = MLC;
    }
}

// Handle IN_STRING state
void handle_in_string(char ch, State *state, FILE *output) {
    if (ch == '"') {
        *state = DEFAULT;
        fprintf(output, "\"");
    } else if (ch == '\n') {
        fprintf(output, "\n");
    } else {
        fprintf(output, "%c", ch);
    }
}

// Handle IN_CHAR state
void handle_in_char(char ch, State *state, FILE *output) {
    fprintf(output, "%c", ch);
    if (ch == '\'') {
        *state = DEFAULT;
    } else if (ch == '\n') {
        fprintf(output, " ");
    }
}
