#ifndef MCALCULATOR_VERSION_4_H_
#define MCALCULATOR_VERSION_4_H_

#include <stdbool.h>
#include <stddef.h>

typedef enum {
    MC4_NO_ERROR,
    MC4_TOO_MANY_TOKENS
} MC4_ErrorCode;

#define NUM_LETTERS 52

struct MC4_VarMap {
    double map[NUM_LETTERS];
};

typedef struct MC4_Result {
    double value;
    MC4_ErrorCode err;
    double vars[NUM_LETTERS];
} MC4_Result;

static const char* MC4_ErrorCode_to_str(MC4_ErrorCode code) {
    switch (code) {
    case MC4_NO_ERROR: return "No Error";
    default: return NULL;
    }
}

static bool MC4_error_occured(MC4_Result* result) {
    return (result->err != MC4_NO_ERROR);
}

struct MC4_Result MC4_evaluate(const char* equ);

#endif
