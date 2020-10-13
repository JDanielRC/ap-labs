#include <stdio.h>
#include <stdlib.h>
#include <inttypes.h>
#include <string.h>
#include "logger.h"
#include <signal.h>
#include <time.h>
#include <errno.h>   


#define WHITESPACE 64
#define EQUALS     65
#define INVALID    66

long current = 0;
long total = 0;

int base64encode(const void* data_buf, size_t dataLength, char* result, size_t resultSize);
int base64decode (char *in, size_t inLen, unsigned char *out, size_t *outLen);
void handler(int signal);


int main(int argc, char** argv){
    signal(SIGUSR1, handler);
    signal(SIGINT, handler);
    if (argc != 3){
        errorf("Input takes two arguments: (--encode/--decode) (file)");
        return 0;
    }
    if (strcmp(argv[1], "--encode") == 0){
        encode(argv[2]);
        return 0;
    } else if (strcmp(argv[1], "--decode") == 0){
        decode(argv[2]);
        return 0;
    } else {
        errorf("Input takes two arguments: (--encode/--decode) (file)");
        return 0;
    }
    return 0;
}

int msleep(long msec)
{
    struct timespec ts;
    int res;

    if (msec < 0)
    {
        errno = EINVAL;
        return -1;
    }

    ts.tv_sec = msec / 10000;
    ts.tv_nsec = (msec % 10000) * 100000;

    do {
        res = nanosleep(&ts, &ts);
    } while (res && errno == EINTR);

    return res;
}

int encode(char* fileName){
    FILE *read = fopen(fileName, "r");
    if (read == NULL){
        errorf("File %s could not be read or does not exist.", fileName);
        return 1; //error
    }
    infof("Encoding...");
    FILE *output;
    char* extension = "-encoded.txt";
    char* newFileName = malloc(strlen(fileName) + strlen(extension));
    strncpy(newFileName, fileName, strlen(fileName) - 4);
    strcat(newFileName, extension);
    output = fopen(newFileName, "w");
    if (!output){
        perror("fopen");
    }
    size_t pos = ftell(read);
    fseek(read, 0, SEEK_END);
    size_t size = ftell(read);
    fseek(read, pos, SEEK_SET);

    char* data = malloc(size);
    fread(data, 1, size, read);

    char* storage = (char*) malloc(sizeof(char) * strlen(data) * 4);

    base64encode(data, size, storage, sizeof(char) * strlen(data) * 4);
    fprintf(output, "%s", storage);
    fclose(read);
    fclose(output);

    infof("Encoded finished succesfully");
    return 0; //sucess
}

int decode(char* fileName){
    FILE *read = fopen(fileName, "r");
    if (read == NULL){
        errorf("File %s could not be read or does not exist.", fileName);
        return 1; //error
    }
    infof("Decoding...");
    FILE *output;
    char* extension = "-decoded.txt";
    char* newFileName = malloc(strlen(fileName) + strlen(extension));
    strncpy(newFileName, fileName, strlen(fileName) - 12);
    strcat(newFileName, extension);
    output = fopen(newFileName, "w");
    if (!output){
        perror("fopen");
    }

    size_t pos = ftell(read);
    fseek(read, 0, SEEK_END);
    size_t size = ftell(read);
    fseek(read, pos, SEEK_SET);

    char* data = malloc(size);
    fread(data, 1, size, read);

    unsigned char* storage = (unsigned char*)malloc(sizeof(char) * size * 4);
    size_t rSize = sizeof(unsigned char *) * size * 4;
    base64decode(data, size, storage, &rSize);
    fprintf(output, "%s", storage);
    fclose(read);
    fclose(output);

    infof("Decoded finished succesfully");
    return 0; //sucess

}

void handler(int signal){
    if (signal == SIGUSR1 || signal == SIGINT){
        infof("Progress: %d%%, remaining: %ld", (int)(100.0-(((current*1.0)/(total*1.0))*100.0)), current);
    }
}


//FUNCTION PULLED FROM https://en.wikibooks.org/wiki/Algorithm_Implementation/Miscellaneous/Base64#C

int base64encode(const void* data_buf, size_t dataLength, char* result, size_t resultSize) {
   const char base64chars[] = "ABCDEFGHIJKLMNOPQRSTUVWXYZabcdefghijklmnopqrstuvwxyz0123456789+/";
   const uint8_t *data = (const uint8_t *)data_buf;
   size_t resultIndex = 0;
   size_t x;
   uint32_t n = 0;
   int padCount = dataLength % 3;
   uint8_t n0, n1, n2, n3;

   total = dataLength;
   current = dataLength;

   for (x = 0; x < dataLength; x += 3) 
   {

      n = ((uint32_t)data[x]) << 16; //parenthesis needed, compiler depending on flags can do the shifting before conversion to uint32_t, resulting to 0
      
      if((x+1) < dataLength)
         n += ((uint32_t)data[x+1]) << 8;//parenthesis needed, compiler depending on flags can do the shifting before conversion to uint32_t, resulting to 0
      
      if((x+2) < dataLength)
         n += data[x+2];

      n0 = (uint8_t)(n >> 18) & 63;
      n1 = (uint8_t)(n >> 12) & 63;
      n2 = (uint8_t)(n >> 6) & 63;
      n3 = (uint8_t)n & 63;

      if(resultIndex >= resultSize) return 1;   /* indicate failure: buffer too small */
      result[resultIndex++] = base64chars[n0];
      if(resultIndex >= resultSize) return 1;   /* indicate failure: buffer too small */
      result[resultIndex++] = base64chars[n1];

      if((x+1) < dataLength)
      {
         if(resultIndex >= resultSize) return 1;   /* indicate failure: buffer too small */
         result[resultIndex++] = base64chars[n2];
      }
      if((x+2) < dataLength)
      {
         if(resultIndex >= resultSize) return 1;   /* indicate failure: buffer too small */
         result[resultIndex++] = base64chars[n3];
      }
   }
   if (padCount > 0) 
   { 
      for (; padCount < 3; padCount++) 
      { 
         if(resultIndex >= resultSize) return 1;   /* indicate failure: buffer too small */
         result[resultIndex++] = '=';
      } 
   }
   if(resultIndex >= resultSize) return 1;   /* indicate failure: buffer too small */
   result[resultIndex] = 0;
   return 0;   /* indicate success */
}


//FUNCTION PULLED FROM https://en.wikibooks.org/wiki/Algorithm_Implementation/Miscellaneous/Base64#C_2

static const unsigned char d[] = {
    66,66,66,66,66,66,66,66,66,66,64,66,66,66,66,66,66,66,66,66,66,66,66,66,66,
    66,66,66,66,66,66,66,66,66,66,66,66,66,66,66,66,66,66,62,66,66,66,63,52,53,
    54,55,56,57,58,59,60,61,66,66,66,65,66,66,66, 0, 1, 2, 3, 4, 5, 6, 7, 8, 9,
    10,11,12,13,14,15,16,17,18,19,20,21,22,23,24,25,66,66,66,66,66,66,26,27,28,
    29,30,31,32,33,34,35,36,37,38,39,40,41,42,43,44,45,46,47,48,49,50,51,66,66,
    66,66,66,66,66,66,66,66,66,66,66,66,66,66,66,66,66,66,66,66,66,66,66,66,66,
    66,66,66,66,66,66,66,66,66,66,66,66,66,66,66,66,66,66,66,66,66,66,66,66,66,
    66,66,66,66,66,66,66,66,66,66,66,66,66,66,66,66,66,66,66,66,66,66,66,66,66,
    66,66,66,66,66,66,66,66,66,66,66,66,66,66,66,66,66,66,66,66,66,66,66,66,66,
    66,66,66,66,66,66,66,66,66,66,66,66,66,66,66,66,66,66,66,66,66,66,66,66,66,
    66,66,66,66,66,66
};

int base64decode (char *in, size_t inLen, unsigned char *out, size_t *outLen) { 
    char *end = in + inLen;
    char iter = 0;
    uint32_t buf = 0;
    size_t len = 0;

    total = (long) end - (long) in;
    
    while (in < end) {

        current = (long)end - (long)in;
        msleep(1);
        unsigned char c = d[*in++];
        
        switch (c) {
        case WHITESPACE: continue;   /* skip whitespace */
        case INVALID:    return 1;   /* invalid input, return error */
        case EQUALS:                 /* pad character, end of data */
            in = end;
            continue;
        default:
            buf = buf << 6 | c;
            iter++; // increment the number of iteration
            if (iter == 4) {
                if ((len += 3) > *outLen) return 1;
                *(out++) = (buf >> 16) & 255;
                *(out++) = (buf >> 8) & 255;
                *(out++) = buf & 255;
                buf = 0; iter = 0;

            }   
        }
    }
   
    if (iter == 3) {
        if ((len += 2) > *outLen) return 1;
        *(out++) = (buf >> 10) & 255;
        *(out++) = (buf >> 2) & 255;
    }
    else if (iter == 2) {
        if (++len > *outLen) return 1;
        *(out++) = (buf >> 4) & 255;
    }

    *outLen = len;
    return 0;
}
