#include "mcalc4.h"
#include "mlogging.h"
#include <assert.h>
#include <ctype.h>
#include <math.h>
#include <stdbool.h>
#include <stddef.h>
#include <string.h>

#define MAX_TOKENS 100

enum TokenType {
    EMPTY,
    NUMBER,
    OPERATOR,
    PAR_LEFT,
    PAR_RIGHT,
    CONSTANT,
    FUNCTION,
    VARIABLE,
};

enum ConstantType {
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
        /* Used for storing the type of a `CONSTANT`.  */
        enum ConstantType const_type;
        /* Used for storing the type of a `FUNCTION`. */
        enum FuncType func_type;
        /* Used for storing the identifier of a `VARIABLE`. */
        char var_symbol;
    };
};

struct TokensList {
    /* List of tokens, capacity is is `MAX_TOKENS` */
    struct Token tokens[MAX_TOKENS];
    /* Current index of tokens array */
    unsigned int tkns_pos;
};

/**
 * @brief Returns a new `TokensList` with position set to 0 and every token set
 * to empty.
 *
 * @return struct TokensList
 */
struct TokensList new_list() {
    struct TokensList list;
    list.tkns_pos = 0;
    for (int i = 0; i < MAX_TOKENS; i++) {
        list.tokens[i].type = EMPTY;
    }
    return list;
}

void add_token(struct TokensList* list, struct Token token,
               MC4_ErrorCode* err) {

    if (list->tkns_pos >= MAX_TOKENS) {
        *err = MC4_TOO_MANY_TOKENS;
        MLOG_error("Too many tokens");
        return;
    } else {
        list->tokens[list->tkns_pos] = token;
        list->tkns_pos++;
    }
}

double read_num(const char* equ, int* index) {
    // TODO: Add error handling for read_num().
    double whole_part = 0.0;
    double decimal_part = 0.0;

    while (isspace(equ[*index])) {
        (*index)++;
    }

    while (isdigit(equ[*index])) {
        whole_part = (whole_part * 10) + (equ[*index] - '0');
        (*index)++;
    }

    double divisor = 10.0;
    if (equ[*index] == '.') {
        (*index)++;

        while (isdigit(equ[*index])) {
            decimal_part += (equ[*index] - '0') / divisor;
            divisor *= 10.0;
            (*index)++;
        }
    }

    (*index)--; /* backtracks to prevent skipping next char */
    return whole_part + decimal_part;
}

bool is_func_str(const char* s, const int index) {
    const char* FUNC_STRS[] = {"sin",    "cos",    "tan", "arcsin",
                               "arccos", "arctan", "log", "ln"};
    const int size = sizeof(FUNC_STRS) / sizeof(FUNC_STRS[0]);

    for (int i = 0; i < size; i++) {
        if (strstr(&s[index], FUNC_STRS[i]) == &s[index]) {
            return true;
        }
    }

    return false;
}

enum FuncType funcstr_to_type(const char* s, int* index) {
    const char* FUNC_STRS[] = {"sin",    "cos",    "tan", "arcsin",
                               "arccos", "arctan", "log", "ln"};

    const enum FuncType FUNC_TYPES[] = {FN_SIN,  FN_COS,  FN_TAN,    FN_ASIN,
                                        FN_ACOS, FN_ATAN, FN_LOG_10, FN_LOG_E};

    const int FN_STRS_SIZE = sizeof(FUNC_STRS) / sizeof(FUNC_STRS[0]);
    const int FN_TYPES_SIZE = sizeof(FUNC_TYPES) / sizeof(FUNC_TYPES[0]);

    assert(FN_STRS_SIZE == FN_TYPES_SIZE);

    for (int i = 0; i < FN_STRS_SIZE; i++) {
        if (strstr(&s[*index], FUNC_STRS[i]) == &s[*index]) {
            *index += strlen(FUNC_STRS[i]);
            return FUNC_TYPES[i];
        }
    }

    return -1;
}

bool is_constant_str(const char* s, const int index) {
    const char* CONSTANTS[] = {"pi", "e"};
    const int size = sizeof(CONSTANTS) / sizeof(CONSTANTS[0]);

    for (int i = 0; i < size; i++) {
        if (strstr(&s[index], CONSTANTS[i]) == &s[index]) {
            return true;
        }
    }

    return false;
}

enum ConstantType conststr_to_type(const char* s, int* index) {
    const char* CONSTANT_STRS[] = {"pi", "e"};

    const enum ConstantType CONSTANT_TYPES[] = {CONST_PI, CONST_E};

    const int FN_STRS_SIZE = sizeof(CONSTANT_STRS) / sizeof(CONSTANT_STRS[0]);
    const int FN_TYPES_SIZE =
        sizeof(CONSTANT_TYPES) / sizeof(CONSTANT_TYPES[0]);

    assert(FN_STRS_SIZE == FN_TYPES_SIZE);

    for (int i = 0; i < FN_STRS_SIZE; i++) {
        if (strstr(&s[*index], CONSTANT_STRS[i]) == &s[*index]) {
            *index += strlen(CONSTANT_STRS[i]);
            return CONSTANT_TYPES[i];
        }
    }

    return -1;
}

/**
 * @brief Takes in string and tokenized it, writing to `list`. An error is
 * written to `err`.
 *
 * @param equ
 * @param list
 * @param err
 */
struct TokensList tokenize(const char* equ, MC4_ErrorCode* err) {
    // TODO: add error handling in tokenize().
    struct TokensList tokens_list = new_list();
    const int EQU_LEN = strlen(equ);

    for (int i = 0; i < EQU_LEN; i++) {
        if (equ[i] == ' ') {
            continue; /* ignore whitespace */
        } else if (strchr("+-*/^", equ[i]) != NULL) {
            add_token(&tokens_list,
                      (struct Token){.type = OPERATOR, .op = equ[i]}, err);
        } else if (isdigit(equ[i])) {
            double value = read_num(equ, &i);
            add_token(&tokens_list,
                      (struct Token){.type = NUMBER, .value = value}, err);
        } else if (equ[i] == '(' || equ[i] == ')') {
            if (equ[i] == '(') {
                add_token(&tokens_list, (struct Token){.type = PAR_LEFT}, err);
            } else {
                add_token(&tokens_list, (struct Token){.type = PAR_RIGHT}, err);
            }
        } else if (is_func_str(equ, i)) {
            enum FuncType func_type = funcstr_to_type(equ, &i);
            add_token(&tokens_list,
                      (struct Token){.type = FUNCTION, .func_type = func_type},
                      err);
        } else if (is_constant_str(equ, i)) {
            enum ConstantType const_type = conststr_to_type(equ, &i);
            add_token(
                &tokens_list,
                (struct Token){.type = CONSTANT, .const_type = const_type},
                err);
        } else if (isalpha(equ[i])) {
            add_token(&tokens_list,
                      (struct Token){.type = VARIABLE, .var_symbol = equ[i]},
                      err);
        } else {
            MLOG_error("Encounted unexpected character in tokenize()");
            return (struct TokensList){
                .tokens = {(struct Token){.type = EMPTY}}, .tkns_pos = -1};
        }
    }

    return tokens_list;
}

double parse(struct TokensList* tokens, MC4_ErrorCode* err);

double MC4_evaluate(const char* equ, MC4_ErrorCode* err) {
    MC4_ErrorCode error_code = MC4_NO_ERROR;

    // TODO: implement tokenization.
    struct TokensList tokens_list = tokenize(equ, &error_code);
    (void)tokens_list;

    if (err != NULL) {
        *err = error_code;
    }

    return 0.0;
}

/* Code below is only for tests */

#include <float.h>

bool doubles_mostly_equal(double a, double b) {
    return fabs(a - b) < DBL_EPSILON;
}

bool tokens_equal(struct Token a, struct Token b) {
    switch (a.type) {
        case EMPTY:
            return b.type == EMPTY;
        case NUMBER:
            return (b.type == NUMBER) &&
                   (doubles_mostly_equal(a.value, b.value));
        case OPERATOR:
            return (b.type == OPERATOR) && (a.op == b.op);
        case PAR_LEFT:
            return b.type == PAR_LEFT;
        case PAR_RIGHT:
            return b.type == PAR_RIGHT;
        case CONSTANT:
            return (b.type == CONSTANT) && (a.const_type == b.const_type);
        case FUNCTION:
            return (b.type == FUNCTION) && (a.func_type == b.func_type);
        case VARIABLE:
            return (b.type == VARIABLE) && (a.var_symbol == b.var_symbol);
        default:
            return false;
    }
}

bool tokens_arr_equal(struct Token a[], struct Token b[], const int len) {
    for (int i = 0; i < len; i++) {
        if (!tokens_equal(a[i], b[i])) {
            return false;
        }
    }

    return true;
}

void print_token(struct Token* token) {
    switch (token->type) {
        case EMPTY:
            printf("EMPTY");
            break;
        case NUMBER:
            printf("NUMBER(%lf)", token->value);
            break;
        case OPERATOR:
            printf("OPERATOR(%c)", token->op);
            break;
        case PAR_LEFT:
            printf("PAR_LEFT");
            break;
        case PAR_RIGHT:
            printf("PAR_RIGHT");
            break;
        case CONSTANT:
            if (token->const_type == CONST_PI) {
                printf("CONST_PI");
            } else if (token->const_type == CONST_E) {
                printf("CONST_E");
            }
            break;
        case FUNCTION:
            switch (token->func_type) {
                case FN_SIN:
                    printf("FN_SIN");
                    break;
                case FN_COS:
                    printf("FN_COS");
                    break;
                case FN_TAN:
                    printf("FN_TAN");
                    break;
                case FN_ASIN:
                    printf("FN_ASIN");
                    break;
                case FN_ACOS:
                    printf("FN_ACOS");
                    break;
                case FN_ATAN:
                    printf("FN_ATAN");
                    break;
                case FN_LOG_10:
                    printf("FN_LOG_10");
                    break;
                case FN_LOG_E:
                    printf("FN_LOG_E");
                    break;
            }
            break;
        case VARIABLE:
            printf("VARIABLE(%c)", token->var_symbol);
            break;
    }
}

void test_tokenization(void) {
    MLOG_log("Tokenization Test Suite");

    struct Token test_one[] = {(struct Token){.type = NUMBER, .value = 2},
                               (struct Token){.type = OPERATOR, .op = '+'},
                               (struct Token){.type = NUMBER, .value = 4}};
    MLOG_test("2 + 4",
              tokens_arr_equal(tokenize("2+4", NULL).tokens, test_one,
                               sizeof(test_one) / sizeof(test_one[0])));
    // MLOG_array_custom(test_one_tkns.tokens, 3, print_token);

    struct Token test_two[] = {(struct Token){.type = PAR_LEFT},
                               (struct Token){.type = NUMBER, .value = 2},
                               (struct Token){.type = OPERATOR, .op = '+'},
                               (struct Token){.type = NUMBER, .value = 4},
                               (struct Token){.type = PAR_RIGHT},
                               (struct Token){.type = OPERATOR, .op = '*'},
                               (struct Token){.type = NUMBER, .value = 6}};
    // struct TokensList test_two_tkns = tokenize("(2+4)*6", NULL);
    MLOG_test("(2 + 4) * 6",
              tokens_arr_equal(tokenize("(2+4)*6", NULL).tokens, test_two,
                               sizeof(test_two) / sizeof(test_two[0])));
}

void test_parsing(void);
