#ifndef MCALCULATOR_VERSION_4_UTILS_H_
#define MCALCULATOR_VERSION_4_UTILS_H_

#include <stddef.h>

#define MAX_TOKENS 1000
#define MC4_VARSET_SIZE 52
#define MC4_VARSET_HALF_SIZE (MC4_VARSET_SIZE / 2)

enum TokenType {
    TYPE_EMPTY,
    TYPE_NUMBER,
    TYPE_OPERATOR,
    TYPE_PAR_LEFT,
    TYPE_PAR_RIGHT,
    TYPE_CONSTANT,
    TYPE_FUNCTION,
    TYPE_VARIABLE,
};

enum ConstType {
    CONST_PI,
    CONST_E,
};

enum FuncType {
    FN_SIN,
    FN_COS,
    FN_TAN,
    FN_ASIN,
    FN_ACOS,
    FN_ATAN,
    FN_LOG_10,
    FN_LOG_E,
};

struct Token {
    /* Stores the type of token. Metadata for token is stored in attached
    union. */
    enum TokenType type;

    union {
        /* Used for storing type of operator for `OPERATOR`. */
        char op;
        /* Used for storing the value of a `NUMBER`. */
        double value;
        /* Used for storing the type of a `FUNCTION`. */
        enum FuncType func_type;
        /* Used for storing the identifier of a `VARIABLE`. */
        char symbol;
    };
};

struct TokensList {
    /* List of tokens, capacity is is `MAX_TOKENS` */
    struct Token tokens[MAX_TOKENS];
    /* Current index of tokens array */
    unsigned int tkns_pos;
};

typedef enum {
    MC4_ERR_NONE,
    MC4_ERR_MAX_TOKENS,
    MC4_ERR_UNEXPECTED_TOKEN,
    MC4_ERR_NUM_FMT_ERR,
    MC4_ERR_VAR_NOT_FOUND,
} MC4_ErrorCode;

struct MC4_VariableSet {
    double values_hashmap[MC4_VARSET_SIZE];
    bool exists_hashmap[MC4_VARSET_SIZE];
};

struct TokensList tokenize(const char* equ, MC4_ErrorCode* err);

#endif
