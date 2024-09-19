#include <stdio.h>

int main(void){
    int c;
    int letterCount = 0;
    int digitCount = 0;
    int otherCount = 0;

    while ((c = getchar()) != EOF){
        if ((c >= 97 && c <= 122) || (c >= 65 && c<= 90)){
            letterCount++;
        } else if (c >= 48 && c <= 57){
            digitCount++;
        } else {
            otherCount++;
        }
    }

    printf("# of letters: %d\n", letterCount);
    printf("# of digits: %d\n", digitCount);
    printf("# of other characters: %d\n", otherCount);
    
    return 0;
    
    
}

/* 


//
*/