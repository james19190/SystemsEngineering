#include <stdio.h>
#include <assert.h>
#include <stdlib.h>

typedef enum {
  DEFAULT,
  COMMENT_START,
  SLC,
  MLC,
  SLC_END,
  MLC_END,
  IN_STRING,
  IN_CHAR
} State;

int main(void){

  // dummy variables
  int ich;
  char ch;
 
  // meta data
  int line_cur = 1;
  int line_com = -1;
  State currentState = DEFAULT; // Start in the DEFAULT state

  ich = getchar();
  
  while (ich != EOF){

    ch = (char) ich;

    switch(currentState){
      case DEFAULT:
        break;
      case COMMENT_START:
        break;
      case SLC:
        break;
      case MLC:
        break;
      case SLC_END:
        break;
      case MLC_END:
        break;
      case IN_STRING:
        break;
      case IN_CHAR:
        break;
    }

    ich = getchar();
  }

  return(EXIT_SUCCESS);

}
