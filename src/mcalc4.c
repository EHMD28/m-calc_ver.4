#include "mcalc4.h"
#include "mlogging.h"
#include "tests.h"
#include <assert.h>
#include <ctype.h>
#include <math.h>
#include <stdbool.h>
#include <stdlib.h>
#include <string.h>

#define MAX_TOKENS 100 // Current 100 for testing purposes, increase in future.
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

struct StringReader {
    const char* str;
    unsigned int pos;
};

static struct StringReader new_string_reader(const char* s) {
    return (struct StringReader){.str = s, .pos = 0};
}

/**
 * @brief Get next character.
 */
static char reader_get_current(struct StringReader* reader) {
    return reader->str[reader->pos];
}

/**
 * @brief Move to next character.
 */
static void reader_advance(struct StringReader* reader) {
    reader->pos++;
}

/**
 * @brief Move back one character.
 */
static void reader_move_back(struct StringReader* reader) {
    reader->pos--;
}

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

const char* consttype_to_str(enum ConstantType type) {
    switch (type) {
    case CONST_PI: return "PI";
    case CONST_E: return "E";
    }
}

const char* functype_to_str(enum FuncType type) {
    switch (type) {
    case FN_SIN: return "SIN";
    case FN_COS: return "COS";
    case FN_TAN: return "TAN";
    case FN_ASIN: return "ASIN";
    case FN_ACOS: return "ACOS";
    case FN_ATAN: return "ATAN";
    case FN_LOG_10: return "LOG_10";
    case FN_LOG_E: return "LOG_E";
    }
}

const char* token_to_str(void* ptr) {
    struct Token* token = ptr;
    static char buffer[100] = {0};
    const char* type = NULL;
    switch (token->type) {
    case TYPE_EMPTY: type = "EMPTY"; break;
    case TYPE_NUMBER: type = "NUMBER"; break;
    case TYPE_OPERATOR: type = "OPERATOR"; break;
    case TYPE_PAR_LEFT: type = "PAR_LEFT"; break;
    case TYPE_PAR_RIGHT: type = "PAR_RIGHT"; break;
    case TYPE_CONSTANT: type = "CONSTANT"; break;
    case TYPE_FUNCTION: type = "FUNCTION"; break;
    case TYPE_VARIABLE: type = "VARIABLE"; break;
    }

    if (token->type == TYPE_NUMBER) {
        snprintf(buffer, 100, "%s(%lf)", type, token->value);
    } else if (token->type == TYPE_OPERATOR) {
        snprintf(buffer, 100, "%s(%c)", type, token->op);
    } else if (token->type == TYPE_CONSTANT) {
        snprintf(buffer, 100, "%s(%s)", type,
                 consttype_to_str(token->const_type));
    } else if (token->type == TYPE_FUNCTION) {
        snprintf(buffer, 100, "%s(%s)", type,
                 functype_to_str(token->func_type));
    } else if (token->type == TYPE_VARIABLE) {
        snprintf(buffer, 100, "%s(%c)", type, token->var_symbol);
    } else {
        snprintf(buffer, 100, "%s", type);
    }

    return buffer;
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
        MLOG.error("Too many tokens.");
        return;
    } else {
        // MLOG_logf("Added token: %s", token_to_str(&token));
        list->tokens[list->tkns_pos] = token;
        list->tkns_pos++;
    }
}

double read_num(struct StringReader* reader) {
    // TODO: Add error handling for read_num().
    double whole_part = 0.0;
    double decimal_part = 0.0;

    while (isdigit(reader_get_current(reader))) {
        whole_part = (whole_part * 10) + (reader_get_current(reader) - '0');
        reader_advance(reader);
    }

    double divisor = 10.0;
    if (reader_get_current(reader) == '.') {
        reader_advance(reader);

        while (isdigit(reader_get_current(reader))) {
            decimal_part += (reader_get_current(reader) - '0') / divisor;
            divisor *= 10;
            reader_advance(reader);
        }
    }
    /* backtracks to prevent skipping next char */
    // reader_move_back(reader);
    return whole_part + decimal_part;
}

bool is_func_str(const struct StringReader* reader) {
    const char* FUNC_STRS[] = {"sin",    "cos",    "tan", "arcsin",
                               "arccos", "arctan", "log", "ln"};
    const int size = sizeof(FUNC_STRS) / sizeof(FUNC_STRS[0]);

    for (int i = 0; i < size; i++) {
        if (string_at(FUNC_STRS[i], reader->str, reader->pos)) {
            return true;
        }
    }

    return false;
}

enum FuncType funcstr_to_type(struct StringReader* reader) {
    const char* FUNC_STRS[] = {"sin",    "cos",    "tan", "arcsin",
                               "arccos", "arctan", "log", "ln"};

    const enum FuncType FUNC_TYPES[] = {FN_SIN,  FN_COS,  FN_TAN,    FN_ASIN,
                                        FN_ACOS, FN_ATAN, FN_LOG_10, FN_LOG_E};

    const int FN_STRS_SIZE = sizeof(FUNC_STRS) / sizeof(FUNC_STRS[0]);
    const int FN_TYPES_SIZE = sizeof(FUNC_TYPES) / sizeof(FUNC_TYPES[0]);

    assert(FN_STRS_SIZE == FN_TYPES_SIZE);

    for (int i = 0; i < FN_STRS_SIZE; i++) {
        if (string_at(FUNC_STRS[i], reader->str, reader->pos)) {
            reader->pos += strlen(FUNC_STRS[i]);
            return FUNC_TYPES[i];
        }
    }

    return -1;
}

bool is_constant_str(struct StringReader* reader) {
    const char* const CONSTANTS[] = {"pi", "e"};
    const int SIZE = sizeof(CONSTANTS) / sizeof(CONSTANTS[0]);

    for (int i = 0; i < SIZE; i++) {
        if (string_at(CONSTANTS[i], reader->str, reader->pos)) {
            return true;
        }
    }

    return false;
}

enum ConstantType conststr_to_type(struct StringReader* reader) {
    const char* CONSTANT_STRS[] = {"pi", "e"};

    const enum ConstantType CONSTANT_TYPES[] = {CONST_PI, CONST_E};

    const int FN_STRS_SIZE = sizeof(CONSTANT_STRS) / sizeof(CONSTANT_STRS[0]);
    const int FN_TYPES_SIZE =
        sizeof(CONSTANT_TYPES) / sizeof(CONSTANT_TYPES[0]);

    assert(FN_STRS_SIZE == FN_TYPES_SIZE);

    for (int i = 0; i < FN_STRS_SIZE; i++) {
        if (string_at(CONSTANT_STRS[i], reader->str, reader->pos)) {
            reader->pos += strlen(CONSTANT_STRS[i]);
            return CONSTANT_TYPES[i];
        }
    }

    return -1;
}

double const_to_value(enum ConstantType type) {
    switch (type) {
    case CONST_PI: return M_PI;
    case CONST_E: return M_E;
    }
}

/**
 * @brief Changes tokens into parseable format (i.e. converting from constants
 * to doubles and handling negative signs).
 *
 * @param list
 */
void fix_tokens(struct TokensList* list) {
    for (unsigned int i = 0; i < list->tkns_pos; i++) {
        if (list->tokens[i].type == TYPE_CONSTANT) {
            double value = const_to_value(list->tokens[i].const_type);
            list->tokens[i].type = TYPE_NUMBER;
            list->tokens[i].value = value;
        }
    }
}

static void reader_handle_whitespace(struct StringReader* reader) {
    while (isspace(reader_get_current(reader))) {
        reader_advance(reader);
    }
}

static void reader_handle_op(struct StringReader* reader,
                             struct TokensList* list) {
    char current_ch = reader_get_current(reader);
    while (strchr("+-*/^", current_ch) != NULL) {
        add_token(list, (struct Token){.type = TYPE_OPERATOR, .op = current_ch},
                  NULL);
        reader_advance(reader);
        current_ch = reader_get_current(reader);
    }
}

static void reader_handle_digit(struct StringReader* reader,
                                struct TokensList* list) {
    if (isdigit(reader_get_current(reader))) {
        double value = read_num(reader);
        add_token(list, (struct Token){.type = TYPE_NUMBER, .value = value},
                  NULL);
    }
}

static void reader_handle_par(struct StringReader* reader,
                              struct TokensList* list) {
    char current_ch = reader_get_current(reader);

    while (current_ch == '(' || current_ch == ')') {
        if (current_ch == '(') {
            add_token(list, (struct Token){.type = TYPE_PAR_LEFT}, NULL);
        } else {
            add_token(list, (struct Token){.type = TYPE_PAR_RIGHT}, NULL);
        }

        reader_advance(reader);
        current_ch = reader_get_current(reader);
    }
}

static bool reader_handle_func(struct StringReader* reader,
                               struct TokensList* list) {
    if (is_func_str(reader)) {
        enum FuncType func_type = funcstr_to_type(reader);
        add_token(list,
                  (struct Token){.type = TYPE_FUNCTION, .func_type = func_type},
                  NULL);
        return true;
    }
    return false;
}

static bool reader_handle_const(struct StringReader* reader,
                                struct TokensList* list) {
    if (is_constant_str(reader)) {
        enum ConstantType const_type = conststr_to_type(reader);
        add_token(
            list,
            (struct Token){.type = TYPE_CONSTANT, .const_type = const_type},
            NULL);
        return true;
    }
    return false;
}

static void reader_handle_var(struct StringReader* reader,
                              struct TokensList* list) {
    if (isalpha(reader_get_current(reader))) {
        add_token(list,
                  (struct Token){.type = TYPE_VARIABLE,
                                 .var_symbol = reader_get_current(reader)},
                  NULL);
        reader_advance(reader);
    }
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
    (void)err;
    struct TokensList tokens_list = new_list();
    const unsigned int EQU_LEN = strlen(equ);
    struct StringReader reader = new_string_reader(equ);

    while (reader.pos < EQU_LEN) {
        reader_handle_whitespace(&reader);
        reader_handle_op(&reader, &tokens_list);
        reader_handle_digit(&reader, &tokens_list);
        reader_handle_par(&reader, &tokens_list);
        if (reader_handle_func(&reader, &tokens_list)) continue;
        if (reader_handle_const(&reader, &tokens_list)) continue;
        /* It is necessary to continue loop to avoid reading functions or
        constants as variables. */
        reader_handle_var(&reader, &tokens_list);
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
    struct Token* current = parser_get_current(parser);

    if (current->type == type) {
        parser->pos++;
    } else {
        MLOG.panicf("Unexpected token: %s. Expected: %s", token_to_str(current),
                    tokens_type_to_str(type));
    }
}

double parse_func(struct Parser* parser, MC4_ErrorCode* err);
double parse_exp(struct Parser* parser, MC4_ErrorCode* err);
double parse_multdiv(struct Parser* parser, MC4_ErrorCode* err);
double parse_addsub(struct Parser* parser, MC4_ErrorCode* err);
double parse_numpar(struct Parser* parser, MC4_ErrorCode* err);

double parse_func(struct Parser* parser, MC4_ErrorCode* err) {
    // TODO: Check if correct.
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

double parse_numpar(struct Parser* parser, MC4_ErrorCode* err) {
    struct Token* current = parser_get_current(parser);
    double value = 0;

    if (current->type == TYPE_NUMBER) {
        value = current->value;
        parser_consume(parser, TYPE_NUMBER);
    } else if (current->type == TYPE_PAR_LEFT) {
        parser_consume(parser, TYPE_PAR_LEFT);
        value = parse_addsub(parser, err);
        parser_consume(parser, TYPE_PAR_RIGHT);
    }

    return value;
}

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
    if (err != NULL) *err = error_code;
    fix_tokens(&tokens_list);

    double result = parse_tokens(&tokens_list, &error_code);
    if (err != NULL) *err = error_code;

    return result;
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

static void test_tokenization_one(void) {
    struct Token test[] = {(struct Token){.type = TYPE_NUMBER, .value = 2},
                           (struct Token){.type = TYPE_OPERATOR, .op = '+'},
                           (struct Token){.type = TYPE_NUMBER, .value = 4}};
    struct TokensList result = tokenize("2+4", NULL);
    int passed = MLOG.test(
        "2 + 4", tokens_arr_equal(result.tokens, test, ARR_SIZE(test)));
    if (!passed) {
        MLOG.array_custom(&result, result.tkns_pos, sizeof(struct Token),
                          &token_to_str);
    }
}

static void test_tokenization_two(void) {
    struct Token test[] = {(struct Token){.type = TYPE_PAR_LEFT},
                           (struct Token){.type = TYPE_NUMBER, .value = 2},
                           (struct Token){.type = TYPE_OPERATOR, .op = '+'},
                           (struct Token){.type = TYPE_NUMBER, .value = 4},
                           (struct Token){.type = TYPE_PAR_RIGHT},
                           (struct Token){.type = TYPE_OPERATOR, .op = '*'},
                           (struct Token){.type = TYPE_NUMBER, .value = 6}};
    struct TokensList result = tokenize("(2+4)*6", NULL);
    int passed = MLOG.test(
        "(2+4)*6", tokens_arr_equal(result.tokens, test, ARR_SIZE(test)));
    if (!passed) {
        MLOG.array_custom(&result, result.tkns_pos, sizeof(struct Token),
                          &token_to_str);
    }
}

static void test_tokenization_three(void) {
    struct Token test[] = {
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
    struct TokensList result = tokenize("(2*4/6)^8", NULL);
    int passed = MLOG.test(
        "(2*4/6)^8", tokens_arr_equal(result.tokens, test, ARR_SIZE(test)));
    if (!passed) {
        MLOG.array_custom(&result, result.tkns_pos, sizeof(struct Token),
                          &token_to_str);
    }
}

static void test_tokenization_four(void) {
    struct Token test[] = {
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
    struct TokensList result = tokenize("cos(arctan(sin(pi/2)))", NULL);
    int passed =
        MLOG.test("cos(arctan(sin(pi/2)))",
                  tokens_arr_equal(result.tokens, test, ARR_SIZE(test)));
    if (!passed) {
        MLOG.array_custom(&result, result.tkns_pos, sizeof(struct Token),
                          &token_to_str);
    }
}

static void test_tokenization_five(void) {
    struct Token test[] = {
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
    struct TokensList result = tokenize("ln(e^2)+log(10)", NULL);
    int passed =
        MLOG.test("ln(e^2)+log(10)",
                  tokens_arr_equal(result.tokens, test, ARR_SIZE(test)));
    if (!passed) {
        MLOG.array_custom(&result, result.tkns_pos, sizeof(struct Token),
                          &token_to_str);
    }
}

void test_tokenization(void) {
    MLOG.log("Tokenization Test Suite");
    test_tokenization_one();
    test_tokenization_two();
    test_tokenization_three();
    test_tokenization_four();
    test_tokenization_five();
}

void test_parsing(void) {
    MLOG.log("Parsing Test Suite");

    double test_one = MC4_evaluate("2+4", NULL);
    MLOG.test("2+4", doubles_mostly_equal(test_one, 6));

    double test_two = MC4_evaluate("(2*4/6)^8", NULL);
    MLOG.test("(2*4/6)^8", doubles_mostly_equal(test_two, 9.98872123151958));

    double test_three = MC4_evaluate("cos(arctan(sin(pi/2)))", NULL);
    MLOG.test("cos(arctan(sin(pi/2)))",
              doubles_mostly_equal(test_three, 0.7071067811865476));

    double test_four = MC4_evaluate("ln(e^2) + log(10)", NULL);
    MLOG.test("ln(e^2) + log(10)", doubles_mostly_equal(test_four, 3));
}
