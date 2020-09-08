#include <stdio.h>
#include <string.h>

int mystrlen(char *string);
char* mystradd(char *origin, char *addition);
int mystrfind(char *origin, char *substr);

int main(int argc, char **argv) {
    if (argc == 4 && strcmp(argv[1], "-add") == 0){
        char* origin = argv[2];
        char* addition = argv[3];
        printf("Initial length: %i \n", mystrlen(origin));
        char* new = mystradd(origin, addition);
        printf("New String: %s \n", new);
        printf("New length: %i \n", mystrlen(new));
    } else if (argc == 4 && strcmp(argv[1], "-find") == 0){
        char* origin = argv[2];
        char* substring = argv[3];
        int position = mystrfind(origin, substring);
        if (position != -1){
            printf("[%s] string was found at [%i] position. \n", substring, position);
        } else {
            printf("[%s] string was not found in the string [%s]. \n", substring, origin);
        }
        
    } else {
        printf("Supported arguments are -add or -find only, along with two strings.\n");
        return 0;
    }
    return 0;
}
