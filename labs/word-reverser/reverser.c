#include <stdio.h>
#include <string.h>

void reverse(char c){
    if (c != '\n'){
        reverse(getchar());
    }
    putchar(c);
    return;
}

void main(int argc, char **argv){
    char c;
    
    while ((c = getchar()) != EOF){
        printf("palabra invertida: ");
        reverse(c);
        putchar('\n');
    }
}