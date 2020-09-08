#include <stdlib.h>

int mystrlen(char *str){
    int i;
    for (i = 0; str[i] != '\0'; i++);
    return i;
}

char *mystradd(char *origin, char *addition){
    char *new = malloc(mystrlen(origin) + mystrlen(addition) + 1);
    int i;
    for (i = 0; origin[i] != '\0'; i++){
        new[i] = origin[i];
    }
    for (int x = 0; addition[x] != '\0'; ++x, ++i){
        new[i] = addition[x];
    }
    return new;
}

int mystrfind(char *origin, char *substr){
    int i = 0, j = 0, firstOcurrence;
    while (origin[i] != '\0'){
        //looping through origin word to find a first coincidence
        while(origin[i] != substr[0] && origin[i] != '\0'){
            i++;
        }

        //not a single coincidence found
        if (origin[i] == '\0'){
            return -1;
        }
        //mark the occurrence place
        firstOcurrence = i;
        //keep comparing chars until we reach the end of any word
        while(origin[i] == substr[j] && origin[i] != '\0' && substr[j] != '\0'){
            i++;
            j++;
        }
        //if we reached the end of the substring first or at the same time, then it is a substring of origin
        if (substr[j] == '\0'){
            return firstOcurrence;
        }
        //otherwise, if we reached the end of origin without finding the end of substring, then it's nto one
        if (origin[i] == '\0'){
            return -1;
        }
        //change our i position to continue where we left of, and j to 0 to compare the whole substring again
        i = firstOcurrence + 1;
        j = 0;


    }
    return 0;
}
