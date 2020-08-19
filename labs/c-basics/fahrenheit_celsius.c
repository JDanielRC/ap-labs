#include <stdio.h>
#include <string.h>
#include <stdlib.h>


/* print Fahrenheit-Celsius table */

int main(int argc, char **argv)
{
    if (argc == 2){
        long fahr = strtol(argv[1], NULL, 10);
        printf("Fahrenheit: %3d, Celcius: %6.1f\n", fahr, (5.0/9.0)*(fahr-32));
    } else if (argc == 4){
        long lower = strtol(argv[1], NULL, 10);
        long upper = strtol(argv[2], NULL, 10);
        long step = strtol(argv[3], NULL, 10);
        for (lower; lower <= upper; lower = lower + step){
            printf("Fahrenheit: %3d, Celcius: %6.1f\n", lower, (5.0/9.0)*(lower-32));
        }
    } else{
        return 0;
    }
}
