#include <stdio.h>
#include <string.h>
#include <stdlib.h>
#include <ctype.h>





void merge(void* array[], int left, int middle, int right, int (*comp) (void *, void *));
void mergeSort(void* array[], int left, int right, int (*comp)(void *, void *));
int numcmp(const char *s1, const char *s2);

int main(int argc, char** argv)
{
    if (argc == 3 && strcmp(argv[2], "numbers.txt") == 0){
        int numeric = 1; //going to compare numbers
        char **array;
        FILE *numbers;
        if ((numbers = fopen("numbers.txt", "r")) == NULL){
            printf("Error opening numbers.txt");
            return 0;
        }
        char next[80]; //string
        int word_alloc = 4;
        int i = 0;
        array = (char **) malloc(sizeof(char*) * word_alloc);
        while (fscanf(numbers, "%s", next) != EOF){
            if (i > word_alloc){
                word_alloc *= 2;
                array = (char **) realloc(array, sizeof(char*) * word_alloc);
            }
            array[i] = (char *) malloc(sizeof(char) * (strlen(next) + 1));
            strcpy(array[i], next);
            ++i;
        }
        mergeSort((void *) array, 0, 99, (int (*)(void*, void*)) (numeric ? numcmp : strcmp));
        for (int j = 0; j < i; j++){
            printf("%s \n", array[j]);
        }
        fclose(numbers);
        return 0;   
    } else if (argc == 2 && strcmp(argv[1], "strings.txt") == 0){
        int numeric = 0; //going to compare strings
        char **array;
        FILE *strings;
        if ((strings = fopen("strings.txt", "r")) == NULL){
            printf("Error opening strings.txt");
            return 0;
        }
        char next[80]; //string
        int word_alloc = 4;
        int i = 0;
        array = (char **) malloc(sizeof(char*) * word_alloc);
        while (fscanf(strings, "%s", next) != EOF){
            if (i > word_alloc){
                word_alloc *= 2;
                array = (char **) realloc(array, sizeof(char*) * word_alloc);
            }
            array[i] = (char *) malloc(sizeof(char) * (strlen(next) + 1));
            strcpy(array[i], next);
            ++i;
        }
        mergeSort((void *) array, 0, 99, (int (*)(void*, void*)) (numeric ? numcmp : strcmp));
        for (int j = 0; j < i; j++){
            printf("%s \n", array[j]);
        }
        fclose(strings);
        return 0;  
    } else{
        printf("Invalid arguments, use numbers.txt or strings.txt to run this program, \n");
    }

    
}

int numcmp(const char *s1, const char *s2) {
    double v1 = atof(s1);
    double v2 = atof(s2);
    if (v1 < v2)
        return -1;
    else if (v1 > v2)
        return 1;
    else
        return 0;
}

void merge(void *array[], int left, int middle, int right, int (*comp)(void *, void *)){
    //printf("merge left %i middle %i right %i \n", left, middle, right);
    int leftSize = middle - left + 1;
    int rightSize = right - middle;

    char *leftArray[leftSize], *rightArray[rightSize];

    for (int i = 0; i < leftSize; i++){
        leftArray[i] = array[left + i];
    }

    for (int i = 0; i < rightSize; i++){
        rightArray[i] = array[middle + i + 1];
    }
    
    //merging temp arrays back into principal array
    int i = 0; //indiex of first subarray
    int j = 0; //index of second subarray
    int k = left; //index of merged subarray
    while (i < leftSize && j < rightSize){
        if ((*comp)(leftArray[i], rightArray[j]) < 0){
            array[k] = leftArray[i];
            i++;
        } else{
            array[k] = rightArray[j];
            j++;
        }
        k++;
    }
    //copy remaining elements
    while (i < leftSize){
        array[k] = leftArray[i];
        i++;
        k++;
    }

    while (j < rightSize){
        array[k] = rightArray[j];
        j++;
        k++;
    }


}

void mergeSort(void *array[], int left, int right, int (*comp)(void *, void *)){
    if (left < right){
        int mid = (left + right) / 2;

        mergeSort(array, left, mid, (*comp));
        mergeSort(array, mid + 1, right, (*comp));

        merge(array, left, mid, right, (*comp));
    }
}
