 #include <stdio.h>
#include <stdlib.h>
#include <string.h>
#include <pthread.h>
#include "logger.h"

long * readMatrix(char *filename);
long * getColumn(int col, long *matrix);
long * getRow(int row, long *matrix);
long * multiply(long *matA, long *matB);
long dotProduct(long *vec1, long *vec2);
int saveResultMatrix(long *result, char* filename);
int getLock();
int releaseLock(int lock);

int NUM_BUFFERS = 0;
long **buffers;
pthread_mutex_t *mutex;
pthread_t threads[2000];

struct args {
    int row;
    int col;
    long* matrixA;
    long* matrixB;
};

int main(int argc, char** argv){
    initLogger("stdout");
    long *Adata, *bData;
    

    if (argc != 5){
        errorf("Incorrect usage. Correct usage is ./multiplier -n [numBuffers] -out [namefile].txt");
        return;
    }

    if (strcmp(argv[1], "-n") != 0){
        errorf("Incorrect usage. Correct usage is ./multiplier -n [numBuffers] -out [namefile].txt");
        return;
    }

    if (strcmp(argv[3], "-out") != 0){
        errorf("Incorrect usage. Correct usage is ./multiplier -n [numBuffers] -out [namefile].txt");
        return;
    }

    infof("Reading matrix files...");
    Adata = readMatrix("./matA.dat");
    bData = readMatrix("./matB.dat");
    infof("Finished!");

    NUM_BUFFERS = atoi(argv[2]);

    if(NUM_BUFFERS <= 0) {
        NUM_BUFFERS = 2;
    }

    char* filename = argv[4];

    buffers = malloc(NUM_BUFFERS * sizeof(long *));
    mutex = malloc(NUM_BUFFERS * sizeof(pthread_mutex_t));

    for(int i = 0; i < NUM_BUFFERS; i++){
        pthread_mutex_init(&mutex[i], NULL);
    }

    infof("Starting data multiplication and file writing...");
    long *matrixResult = multiply(Adata, bData);
    saveResultMatrix(matrixResult, filename);
    infof("Finished!");
    free(buffers);
    free(mutex);
    free(Adata);
    free(bData);
    free(matrixResult);
    return 0;
}

long * readMatrix(char *filename) {
    long nlines = 0;
    long* data;
    FILE *file = fopen(filename, "r");
    if(file == NULL){
        panicf("Filename %s was not found!", filename);
        return NULL;
    }
    char c;
    while((c = fgetc(file)) != EOF){
        if(c=='\n')
            nlines++;
    }

    rewind(file);
    data = malloc(nlines* sizeof(long));
    if(data == NULL){
        return NULL;
    }
    int cont = 0;
    long test;
    while(fscanf(file, "%ld", &data[cont]) != EOF){
        cont++;
    }
    fclose(file);
    return data;
}

long * getColumn(int col, long *matrix) {
    long *column = malloc(2000 * sizeof(long));
    for(int i = 0; i < 2000; i++){
        column[i] = matrix[i*2000 + col];
    }
    return column;
}

long * getRow(int r, long *matrix) {
    long *row = malloc(2000 * sizeof(long));
    for(int i = 0; i < 2000; i++){
        row[i] = matrix[r*2000 + i];
    }
    return row;
}

int getLock() {
    for(int i = 0; i < NUM_BUFFERS; i++){
        if(pthread_mutex_trylock(&mutex[i])==0){
            return i;
        }   
    }
    return -1;
}

int releaseLock(int lock) {
    return pthread_mutex_unlock(&mutex[lock]);
}

long dotProduct(long *vec1, long *vec2) {
    long sum = 0;
    for(int i = 0; i < 2000; i ++) {
        sum += (vec1[i] * vec2[i]);
    }
    return sum;
}

long doMultiplication(struct args *data){
    int bufA = -1, bufB = -1;
    while(bufA == -1 || bufB == -1){
        if(bufA == -1){
            bufA = getLock();
        }
        if(bufB == -1){
            bufB = getLock();
        }
    }
    buffers[bufA] = getRow(data->row, data->matrixA);
    buffers[bufB] = getColumn(data->col, data->matrixB);
    long res = dotProduct(buffers[bufA], buffers[bufB]);
    free(buffers[bufA]);
    free(buffers[bufB]);
    free(data);
    releaseLock(bufA);
    releaseLock(bufB);
    return res;
}

long * multiply(long *matrixA, long *matrixB) {
    long *data = malloc(4000000 * sizeof(long));
    for(int i = 0; i < 2000; i++){

        for(int j = 0; j < 2000; j++)
        {
            struct args *curr_args = malloc(sizeof(struct args));
            curr_args->row = i;
            curr_args->col = j;
            curr_args->matrixA = matrixA;
            curr_args->matrixB = matrixB;
            pthread_create(&threads[j], NULL, (void * (*)(void *))doMultiplication, (void *)curr_args);
        }

        for(int j = 0; j < 2000; j++) {
            void *status;
            pthread_join(threads[j], &status);
            data[2000*j+i] = (long*) status;
        }
    }

    return data;
}

int saveResultMatrix(long *result, char* filename) {    

    FILE *file;
    file = fopen(filename, "w+");
    for(int i = 0; i < 4000000; i++) {
        fprintf(file, "%ld\n", result[i]);
    }
    fclose(file);
    return -1;
}

