#include <stdio.h>
#include <stdlib.h>

int main(int argc, char *argv[]) {
    int year = atoi(argv[1]);
    int isleapyear = 0;
	if (year % 4 == 0) {
        if (year % 100 == 0) {
            if (year % 400 == 0) {
                isleapyear = 1;
            }
        }
        else {
            isleapyear = 1;
        }
    }
    if (isleapyear) {
        printf("%d was a leap year", year);
    }
    else
    {
        printf("%d was not a leap year", year);
    }
	return 0;    
}