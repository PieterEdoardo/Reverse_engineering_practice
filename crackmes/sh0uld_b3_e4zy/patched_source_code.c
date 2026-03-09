#include <stdio.h>
#include <string.h>

int main(void) {
    char input_buffer[6];

    char letter_1 = '\x61'; // 'a'
    char letter_2 = '\x65'; // 'e'
    char letter_3 = '\x69'; // 'i'
    char letter_4 = '\x6f'; // 'o'
    char letter_5 = '\x75'; // 'u'

    char target[6] = {letter_1, letter_2, letter_3, letter_4, letter_5, '\0'};

    puts("MAX INPT 5 CHARS");
    printf("Enter Input: ");

    int input_succeeded = scanf("%5s", input_buffer);

    if (input_succeeded == 1) {
        size_t input_length = strlen(input_buffer);

        if (5 < (int)input_length) {
            puts("INVALID INPUT!");
            return 0;
        }

        if (strcmp(input_buffer, target) == 0) {
            puts("Correct!");
            return 0;
        }
    }

    puts("INVALID INPUT!");
    return 0;
}
