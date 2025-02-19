#include "mcalc4/mcalc4.h"
#include <stdbool.h>
#include <stdio.h>

#define BUFFER_SIZE 512

int main(const int argc, const char* argv[]) {
    MC4_ErrorCode error_code;
    double result;

    /* if `mcacl4` has no command_line arguments */
    if (argc > 1) {
        for (int i = 1; i < argc; i++) {
            result = MC4_evaluate(argv[i], &error_code);

            if (MC4_error_occured(error_code)) {
                printf("%s = ERROR\nAn Error occured\n", argv[i]);
                // TODO: implement error handling in main
            } else {
                printf("%s = %lf", argv[i], result);
            }
        }
    }
    /* start prompt */
    else {
        bool has_exited = false;
        char buffer[BUFFER_SIZE] = {0};

        while (!has_exited) {
            printf("mcalc4> ");
            fgets(buffer, BUFFER_SIZE, stdin);
            printf("%s\n", buffer);
        }
    }
}
