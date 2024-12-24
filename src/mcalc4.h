#ifndef MCALCULATOR_VERSION_4_H_
#define MCALCULATOR_VERSION_4_H_

#include <stdbool.h>
#include <stddef.h>

typedef enum {
    MC4_NO_ERROR,
    MC4_TOO_MANY_TOKENS
} MC4_ErrorCode;

static const char* MC4_ErrorCode_to_str(MC4_ErrorCode code) {
    switch (code) {
        case MC4_NO_ERROR:
            return "No Error";
        default:
            return NULL;
    }
}

static bool MC4_error_occured(MC4_ErrorCode code) {
    return (code != MC4_NO_ERROR);
}

double MC4_evaluate(const char* equ, MC4_ErrorCode* err);

/* tests */
void test_tokenization(void);
void test_parsing(void);

#endif
