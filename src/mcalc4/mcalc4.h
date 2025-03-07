#ifndef MCALCULATOR_VERSION_4_H_
#define MCALCULATOR_VERSION_4_H_

#ifndef M_PI
#define M_PI 3.14159265358979323846
#endif

#ifndef M_E
#define M_E 2.71828182845904523536
#endif

#include <stdbool.h>
#include <stddef.h>

typedef enum {
    MC4_ERR_NONE,
    MC4_ERR_MAX_TOKENS,
    MC4_ERR_VAR_NOT_FOUND,
} MC4_ErrorCode;

#define MC4_VARSET_SIZE 52
#define MC4_VARSET_HALF_SIZE (MC4_VARSET_SIZE / 2)

struct MC4_VariableSet {
    double values_hashmap[MC4_VARSET_SIZE];
    bool exists_hashmap[MC4_VARSET_SIZE];
};

static struct MC4_VariableSet new_varset() {
    return (struct MC4_VariableSet) {
        .values_hashmap = { 0 },
        .exists_hashmap = { false },
    };
}
 
void set_var(struct MC4_VariableSet* vars, char var, double value);

typedef struct MC4_Result {
    double value;
    MC4_ErrorCode err;
    struct MC4_VariableSet vars;
} MC4_Result;

static MC4_Result new_result() {
    return (MC4_Result) {
        .value = 0,
        .err = MC4_ERR_NONE,
        .vars = new_varset(),
    };
}

static const char* MC4_ErrorCode_to_str(MC4_ErrorCode code) {
    switch (code) {
    case MC4_ERR_NONE: return "No Error";
    default: return NULL;
    }
}

static bool MC4_error_occured(MC4_Result* result) {
    return (result->err != MC4_ERR_NONE);
}

struct MC4_Result MC4_evaluate(const char* equ, struct MC4_VariableSet* vars);

#endif
