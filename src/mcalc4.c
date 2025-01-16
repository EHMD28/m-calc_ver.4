#include "mcalc4.h"
#include "mlogging.h"
#include "tests.h"
#include <assert.h>
#include <ctype.h>
#include <math.h>
#include <stdbool.h>
#include <stdlib.h>
#include <string.h>

#define MAX_TOKENS 100
#define ARR_SIZE(arr) ((sizeof(arr)) / (sizeof(arr[0])))

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

const char* tokens_type_to_str(enum TokenType type) {
    switch (type) {
        case TYPE_EMPTY: return "EMPTY";
        case TYPE_NUMBER: return "NUMBER";
        case TYPE_OPERATOR: return "OPERATOR";
        case TYPE_PAR_LEFT: return "PAR_LEFT";
        case TYPE_PAR_RIGHT: return "PAR_RIGHT";
        case TYPE_CONSTANT: return "CONSTANT";
        case TYPE_FUNCTION: return "FUNCTION";
        case TYPE_VARIABLE: return "VARIABLE";
        default: return NULL;
    }
}

/**
 * @brief Checks if `s1` is in `s2` at index `i`
 */
bool string_at(const char* s1, const char* s2, int i) {
    return (strstr(&s2[i], s1) == &s2[i]);
}

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
        list.tokens[i].type = TYPE_EMPTY;
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
        if (string_at(FUNC_STRS[i], s, *index)) {
            /* Minus one to avoid missing next character. */
            *index += strlen(FUNC_STRS[i]) - 1;
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
        if (string_at(CONSTANT_STRS[i], s, *index)) {
            *index += strlen(CONSTANT_STRS[i]) - 1;
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
                      (struct Token){.type = TYPE_OPERATOR, .op = equ[i]}, err);
        } else if (isdigit(equ[i])) {
            double value = read_num(equ, &i);
            add_token(&tokens_list,
                      (struct Token){.type = TYPE_NUMBER, .value = value}, err);
        } else if (equ[i] == '(' || equ[i] == ')') {
            if (equ[i] == '(') {
                add_token(&tokens_list, (struct Token){.type = TYPE_PAR_LEFT},
                          err);
            } else {
                add_token(&tokens_list, (struct Token){.type = TYPE_PAR_RIGHT},
                          err);
            }
        } else if (is_func_str(equ, i)) {
            enum FuncType func_type = funcstr_to_type(equ, &i);
            add_token(
                &tokens_list,
                (struct Token){.type = TYPE_FUNCTION, .func_type = func_type},
                err);
        } else if (is_constant_str(equ, i)) {
            enum ConstantType const_type = conststr_to_type(equ, &i);
            add_token(
                &tokens_list,
                (struct Token){.type = TYPE_CONSTANT, .const_type = const_type},
                err);
        } else if (isalpha(equ[i])) {
            add_token(
                &tokens_list,
                (struct Token){.type = TYPE_VARIABLE, .var_symbol = equ[i]},
                err);
        } else {
            MLOG_error("Encounted unexpected character in tokenize()");
            return (struct TokensList){
                .tokens = {(struct Token){.type = TYPE_EMPTY}}, .tkns_pos = -1};
        }
    }

    return tokens_list;
}

struct Parser {
    struct Token* tokens;
    unsigned int pos;
};

struct Parser new_parser(struct TokensList* list) {
    return (struct Parser){.tokens = list->tokens, .pos = 0};
}

struct Token* parser_get_current(struct Parser* parser) {
    return &parser->tokens[parser->pos];
}

void parser_consume(struct Parser* parser, enum TokenType type) {
    enum TokenType current_type = parser_get_current(parser)->type;

    if (current_type == type) {
        parser->pos++;
    } else {
        MLOG_panicf("Unexpected token: %s", tokens_type_to_str(current_type));
    }
}

double parse_func(struct Parser* parser, MC4_ErrorCode* err);
double parse_exp(struct Parser* parser, MC4_ErrorCode* err);
double parse_multdiv(struct Parser* parser, MC4_ErrorCode* err);
double parse_addsub(struct Parser* parser, MC4_ErrorCode* err);
double parse_numpar(struct Parser* parser, MC4_ErrorCode* err);

double parse_func(struct Parser* parser, MC4_ErrorCode* err) {
    struct Token* current = parser_get_current(parser);
    double value = 0.0;
    while (current->type == TYPE_FUNCTION) {
        parser_consume(parser, TYPE_FUNCTION);
        switch (current->func_type) {
            case FN_SIN: value = sin(parse_numpar(parser, err));
            case FN_COS: value = cos(parse_numpar(parser, err));
            case FN_TAN: value = tan(parse_numpar(parser, err));
            case FN_ASIN: value = asin(parse_numpar(parser, err));
            case FN_ACOS: value = acos(parse_numpar(parser, err));
            case FN_ATAN: value = atan(parse_numpar(parser, err));
            case FN_LOG_10: value = log10(parse_numpar(parser, err));
            case FN_LOG_E: value = log(parse_numpar(parser, err));
        }
        current = parser_get_current(parser);
    }
    return value;
}

double parse_exp(struct Parser* parser, MC4_ErrorCode* err) {
    double value = parse_numpar(parser, err);

    struct Token* current = parser_get_current(parser);
    while ((current->type == TYPE_OPERATOR) && (current->op == '^')) {
        parser_consume(parser, TYPE_OPERATOR);
        value = pow(value, parse_func(parser, err));
        current = parser_get_current(parser);
    }

    return value;
}

double parse_multdiv(struct Parser* parser, MC4_ErrorCode* err) {
    double value = parse_numpar(parser, err);

    struct Token* current = parser_get_current(parser);
    while ((current->type == TYPE_OPERATOR) &&
           (current->op == '*' || current->op == '/')) {
        parser_consume(parser, TYPE_OPERATOR);
        if (current->op == '*') {
            value *= parse_exp(parser, err);
        } else {
            value /= parse_exp(parser, err);
        }

        current = parser_get_current(parser);
    }

    return value;
}

double parse_addsub(struct Parser* parser, MC4_ErrorCode* err) {
    double value = parse_numpar(parser, err);

    struct Token* current = parser_get_current(parser);
    while ((current->type == TYPE_OPERATOR) &&
           (current->op == '+' || current->op == '-')) {
        parser_consume(parser, TYPE_OPERATOR);
        if (current->op == '+') {
            value += parse_multdiv(parser, err);
        } else {
            value -= parse_multdiv(parser, err);
        }
        current = parser_get_current(parser);
    }

    return value;
}

double parse_numpar(struct Parser* parser, MC4_ErrorCode* err) { return 0; }

/**
 * @brief Takes in a list of tokens, and parses the results, returning
 * the result of the expression as a double.
 *
 * @param tokens
 * @param err
 * @return double
 */
double parse_tokens(struct TokensList* list, MC4_ErrorCode* err) {
    (void)err;
    struct Parser parser = new_parser(list);
    /* recursive descent parser starts in terms of lowest order of operations */
    double result = parse_addsub(&parser, err);
    return result;
}

double MC4_evaluate(const char* equ, MC4_ErrorCode* err) {
    MC4_ErrorCode error_code = MC4_NO_ERROR;

    struct TokensList tokens_list = tokenize(equ, &error_code);
    if (err != NULL)
        *err = error_code;

    double result = parse_tokens(&tokens_list, &error_code);
    if (err != NULL)
        *err = error_code;

    return 0.0;
}

/* Code below is only for tests */

#include <float.h>

bool doubles_mostly_equal(double a, double b) {
    return fabs(a - b) < DBL_EPSILON;
}

bool tokens_equal(struct Token a, struct Token b) {
    switch (a.type) {
        case TYPE_EMPTY: return b.type == TYPE_EMPTY;
        case TYPE_NUMBER:
            return (b.type == TYPE_NUMBER) &&
                   (doubles_mostly_equal(a.value, b.value));
        case TYPE_OPERATOR: return (b.type == TYPE_OPERATOR) && (a.op == b.op);
        case TYPE_PAR_LEFT: return b.type == TYPE_PAR_LEFT;
        case TYPE_PAR_RIGHT: return b.type == TYPE_PAR_RIGHT;
        case TYPE_CONSTANT:
            return (b.type == TYPE_CONSTANT) && (a.const_type == b.const_type);
        case TYPE_FUNCTION:
            return (b.type == TYPE_FUNCTION) && (a.func_type == b.func_type);
        case TYPE_VARIABLE:
            return (b.type == TYPE_VARIABLE) && (a.var_symbol == b.var_symbol);
        default: return false;
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

/* Used for debugging purposes only. */
void print_token(struct Token* token) {
    switch (token->type) {
        case TYPE_EMPTY: printf("EMPTY"); break;
        case TYPE_NUMBER: printf("NUMBER(%lf)", token->value); break;
        case TYPE_OPERATOR: printf("OPERATOR(%c)", token->op); break;
        case TYPE_PAR_LEFT: printf("PAR_LEFT"); break;
        case TYPE_PAR_RIGHT: printf("PAR_RIGHT"); break;
        case TYPE_CONSTANT:
            if (token->const_type == CONST_PI) {
                printf("CONST_PI");
            } else if (token->const_type == CONST_E) {
                printf("CONST_E");
            }
            break;
        case TYPE_FUNCTION:
            switch (token->func_type) {
                case FN_SIN: printf("FN_SIN"); break;
                case FN_COS: printf("FN_COS"); break;
                case FN_TAN: printf("FN_TAN"); break;
                case FN_ASIN: printf("FN_ASIN"); break;
                case FN_ACOS: printf("FN_ACOS"); break;
                case FN_ATAN: printf("FN_ATAN"); break;
                case FN_LOG_10: printf("FN_LOG_10"); break;
                case FN_LOG_E: printf("FN_LOG_E"); break;
            }
            break;
        case TYPE_VARIABLE: printf("VARIABLE(%c)", token->var_symbol); break;
    }
}

void test_tokenization(void) {
    MLOG_log("Tokenization Test Suite");

    struct Token test_one[] = {(struct Token){.type = TYPE_NUMBER, .value = 2},
                               (struct Token){.type = TYPE_OPERATOR, .op = '+'},
                               (struct Token){.type = TYPE_NUMBER, .value = 4}};
    MLOG_test("2 + 4",
              tokens_arr_equal(tokenize("2+4", NULL).tokens, test_one,
                               sizeof(test_one) / sizeof(test_one[0])));

    struct Token test_two[] = {(struct Token){.type = TYPE_PAR_LEFT},
                               (struct Token){.type = TYPE_NUMBER, .value = 2},
                               (struct Token){.type = TYPE_OPERATOR, .op = '+'},
                               (struct Token){.type = TYPE_NUMBER, .value = 4},
                               (struct Token){.type = TYPE_PAR_RIGHT},
                               (struct Token){.type = TYPE_OPERATOR, .op = '*'},
                               (struct Token){.type = TYPE_NUMBER, .value = 6}};
    MLOG_test("(2 + 4) * 6",
              tokens_arr_equal(test_two, tokenize("(2+4)*6", NULL).tokens,
                               ARR_SIZE(test_two)));

    /* (2 * 4 / 6) ^ 8 */
    struct Token test_three[] = {
        (struct Token){.type = TYPE_PAR_LEFT},
        (struct Token){.type = TYPE_NUMBER, .value = 2},
        (struct Token){.type = TYPE_OPERATOR, .op = '*'},
        (struct Token){.type = TYPE_NUMBER, .value = 4},
        (struct Token){.type = TYPE_OPERATOR, .op = '/'},
        (struct Token){.type = TYPE_NUMBER, .value = 6},
        (struct Token){.type = TYPE_PAR_RIGHT},
        (struct Token){.type = TYPE_OPERATOR, .op = '^'},
        (struct Token){.type = TYPE_NUMBER, .value = 8},
    };
    MLOG_test("(2 * 4 / 6) ^ 8",
              tokens_arr_equal(test_three, tokenize("(2*4/6)^8", NULL).tokens,
                               ARR_SIZE(test_three)));

    struct Token test_four[] = {
        (struct Token){.type = TYPE_FUNCTION, .func_type = FN_COS},
        (struct Token){.type = TYPE_PAR_LEFT},
        (struct Token){.type = TYPE_FUNCTION, .func_type = FN_ATAN},
        (struct Token){.type = TYPE_PAR_LEFT},
        (struct Token){.type = TYPE_FUNCTION, .func_type = FN_SIN},
        (struct Token){.type = TYPE_PAR_LEFT},
        (struct Token){.type = TYPE_CONSTANT, .const_type = CONST_PI},
        (struct Token){.type = TYPE_OPERATOR, .op = '/'},
        (struct Token){.type = TYPE_NUMBER, .value = 2},
        (struct Token){.type = TYPE_PAR_RIGHT},
        (struct Token){.type = TYPE_PAR_RIGHT},
        (struct Token){.type = TYPE_PAR_RIGHT},
    };
    MLOG_test("cos(arctan(sin(pi/2)))",
              tokens_arr_equal(test_four,
                               tokenize("cos(arctan(sin(pi/2)))", NULL).tokens,
                               ARR_SIZE(test_four)));

    struct Token test_five[] = {
        (struct Token){.type = TYPE_FUNCTION, .func_type = FN_LOG_E},
        (struct Token){.type = TYPE_PAR_LEFT},
        (struct Token){.type = TYPE_CONSTANT, .const_type = CONST_E},
        (struct Token){.type = TYPE_OPERATOR, .op = '^'},
        (struct Token){.type = TYPE_NUMBER, .value = 2},
        (struct Token){.type = TYPE_PAR_RIGHT},
        (struct Token){.type = TYPE_OPERATOR, .op = '+'},
        (struct Token){.type = TYPE_FUNCTION, .func_type = FN_LOG_10},
        (struct Token){.type = TYPE_PAR_LEFT},
        (struct Token){.type = TYPE_NUMBER, .value = 10},
        (struct Token){.type = TYPE_PAR_RIGHT},
    };
    MLOG_test("ln(e^2) + log(10)",
              tokens_arr_equal(test_five,
                               tokenize("ln(e^2)+log(10)", NULL).tokens,
                               ARR_SIZE(test_five)));
}

void test_parsing(void) {
    MLOG_log("Parsing Test Suite");

    double test_one = MC4_evaluate("2+4", NULL);
    MLOG_test("2+4", doubles_mostly_equal(test_one, 6));

    double test_two = MC4_evaluate("(2*4/6)^8", NULL);
    MLOG_test("(2*4/6)^8", doubles_mostly_equal(test_two, 9.98872123151958));

    double test_three = MC4_evaluate("cos(arctan(sin(pi/2)))", NULL);
    MLOG_test("cos(arctan(sin(pi/2)))",
              doubles_mostly_equal(test_three, 0.7071067811865476));

    double test_four = MC4_evaluate("ln(e^2) + log(10)", NULL);
    MLOG_test("ln(e^2) + log(10)", doubles_mostly_equal(test_four, 3))
}
