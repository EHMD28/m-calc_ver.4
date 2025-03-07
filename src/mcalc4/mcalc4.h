#ifndef MCALCULATOR_VERSION_4_H_
#define MCALCULATOR_VERSION_4_H_

#include <ctype.h>
#ifndef M_PI
#define M_PI 3.14159265358979323846
#endif

#ifndef M_E
#define M_E 2.71828182845904523536
#endif

#include <stdbool.h>
#include <stddef.h>
#include "../../libs/mlogging.h"
#include "mcalc4_types.h"

static struct MC4_VariableSet new_varset() {
    return (struct MC4_VariableSet){
        .values_hashmap = {0},
        .exists_hashmap = {false},
    };
}

static int letter_to_key(char ch) {
    if (isupper(ch))
        return ch - 'A';
    else if (islower(ch)) {
        return (ch - 'a') + MC4_VARSET_HALF_SIZE;
    } else {
        MLOG.errorf("letter_to_key() - invalid letter: %c", ch);
        return 0;
    }
}

static char key_to_letter(int key) {
    if ((key >= 0) && (key < MC4_VARSET_HALF_SIZE)) {
        return key + 'A';
    } else if ((key >= MC4_VARSET_HALF_SIZE) && (key < MC4_VARSET_SIZE)) {
        return key - MC4_VARSET_HALF_SIZE + 'a';
    } else {
        MLOG.errorf("key_to_letter() - invalid key: %d", key);
        return 0;
    }
}

static void set_var(struct MC4_VariableSet* vars, char var, double value) {
    int key = letter_to_key(var);
    vars->exists_hashmap[key] = true;
    vars->values_hashmap[key] = value;
}

typedef struct MC4_Result {
    double value;
    MC4_ErrorCode err;
    struct MC4_VariableSet vars;
} MC4_Result;

static MC4_Result new_result() {
    return (MC4_Result){
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
