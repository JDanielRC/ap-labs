#include <stdio.h>
#include <stdlib.h>


static int monthDays[2][13] = {  //dias en cada mes, normal y bisiesto
    {0,31,28,31,30,31,30,31,31,30,31,30,31},
    {0,31,29,31,30,31,30,31,31,30,31,30,31}
};

char *monthName(int n){
    static char *month[] = {
        "Blank Month, Invalid", "January", "February", "March", "April",
        "May", "June", "JUly", "August", "September", "October",
        "November", "December"
    };
    if (n < 1 || n > 12){
        printf("Invalid month number. \n");
    } else{
        return month[n];
    }
    
}

void month_day(int year, int yearday, int *currentMonth, int *currentDay){
    int leap;
    *currentMonth = 1;
    leap = year%4 == 0 && year%100 != 0 || year%400 == 0;
    if (yearday > 365 && leap == 0 || yearday <= 0){
        printf("Year day is out of range in that year. \n");
        return;
    } else if (yearday > 366 && leap == 1 || yearday <= 0){
        printf("Year day is out of range in that year. \n");
        return;
    }
    while (yearday > monthDays[leap][*currentMonth] && *currentMonth < 12){
        *currentDay = monthDays[leap][*currentMonth]; //get number of days of that month
        yearday -= *currentDay; //substract that amount of days from the input
        *currentMonth += 1; //increase current month
    }

    char *monthString = monthName(*currentMonth);
    printf("%s %i, %i. \n", monthString, yearday, year);
}

int main(int argc, char **argv) {
    if (argc < 3 || argc > 4){
        printf("Incorrect number of arguments, you only need two (Year-Yearday).\n");
        return 0; 
    }
    int currentMonth = 0;
    int currentDay = 0;

    int year = atoi(argv[1]);
    int yearDay = atoi(argv[2]);
    month_day(year, yearDay, &currentMonth, &currentDay);
    return 0;
}
