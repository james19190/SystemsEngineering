#include <stdio.h>
#include <assert.h>
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

int main(void){

  int ich;
  char ch;

  State currentState = DEFAULT; // Start in the DEFAULT state

  ich = getchar();
  
  while (ich != EOF){

    ch = (char) ich;

    switch(currentState){

      case DEFAULT:
        if (ch == '/'){
          currentState = COMMENT_START;
        }
        else if (ch == '"'){
          currentState = IN_STRING;
        }
        else if (ch == '\''){
          currentState = IN_CHAR;
        }
        else{
          printf("%c", ch); 
        }
        break;

      case COMMENT_START:
        if (ch == '/'){
          currentState = SLC;
        }
        else if (ch == '*'){
          currentState = MLC;
        }
        else{
          printf("%c", '/'); // Restore '/' if not part of a comment
          printf("%c", ch);
          currentState = DEFAULT;
        }
        break;

      case SLC: 
        // Ignore characters until a newline is found
        if (ch == '\n'){
          printf(" "); // Print space to show comment has been removed
          printf("\n"); // Preserve line numbering by printing a blank line
          currentState = DEFAULT;
        }
        break;

      case MLC:
        // Preserve line numbers within multi-line comments
        if (ch == '\n') {
          printf("\n"); // Print a newline for each newline within the comment
        }
        else if (ch == '*'){
          currentState = MLC_END;
        }
        break;

      case MLC_END:
        if (ch == '/'){
          currentState = DEFAULT;
          printf(" "); // Print space as comment has ended
        } 
        else {
          currentState = MLC;
        }
        break;

      case IN_STRING:
        // Handle string literals, allowing escaped quotes
        printf("%c", ch);
        if (ch == '"'){
          currentState = DEFAULT;
        }
        break;

      case IN_CHAR:
        // Handle character literals, allowing escaped characters
        printf("%c", ch);
        if (ch == '\''){
          currentState = DEFAULT;
        }
        break;
    }

    ich = getchar();
  }

  if (currentState == SLC || currentState == MLC || currentState == MLC_END){
    return(EXIT_FAILURE);
  }


  return(EXIT_SUCCESS);

}
