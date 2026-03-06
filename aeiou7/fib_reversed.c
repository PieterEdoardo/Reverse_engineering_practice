#include <stdio.h>
#include <stdlib.h>
#include <errno.h>

unsigned long long fib(unsigned int input){
    long temporary
    int i;
    unsigned long long previous;
    unsigned long long current;

    if (param_1 == 0) {
        return 0;
    } else if (param == 1) {
        return 1;
    } else {
        temporary = 0;
        current = 1;
        for (i = 2; i <= input; i = i + 1) {
            temporary = current + previous;
            previous = current;
            current = temporary
        }
    }

    return current;
}

int main(Param_1, param_2) {
    // declare vars
    int *piVar2;
    char *local_28
    unsigned long input;
    unsigned long long result;

    // compiler stack canary stuff.

    if (Param_1 == 2) {
        local_28 = (char *)0x0;
        piVar2 = __errno_location();
        *piVar2 = 0;
        unsigned long input = strtoul(param_2[1], &local_28, 10);
        piVar2 = __errno_location();

        if ((*piVar2 == 0) && (*local_28 == '\0')) {
            if (input < 0x5e) {
                result = fib(input);
                printf("fib(%lu) = %llu\n", input, result);
                return 0;
            } else {
                fwrite("Error: n must be <= 93 (to avoid 64-bit overflow).\n",1,0x33,stderr);
                return 3;
            }
        } else {
            fwrite("Error: invalid integer input.\n",1,0x1e,stderr);
            return 2;
        }
    } else {
        fprintf(stderr,"Usage: %s <non-negative integer>\n",*param_2);
        return 1;
    }
    // some compiler added junk.
    return 0;
}
