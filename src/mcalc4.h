#ifndef MCALCULATOR_VERSION_4_H_
#define MCALCULATOR_VERSION_4_H_

typedef enum {
    NO_ERROR,
} MC4_ErrorCode;

double MC4_evaluate(const char* equ, MC4_ErrorCode* err);

#endif
