#include "mcalc4.h"
#include <stdio.h>

int main(const int argc, const char* argv[]) {
    MC4_ErrorCode error_code;
    double result;

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
