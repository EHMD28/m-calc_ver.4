#include "mcalc4.h"
#include "../../libs/mlogging.h"
// #include "../../tests/tests.h"
#include "../cli/cli_types.h"
#include "mcalc4_types.h"
#include <assert.h>
#include <ctype.h>
#include <float.h>
#include <math.h>
#include <stdbool.h>
#include <stdlib.h>
#include <string.h>

#define ARR_SIZE(arr) ((sizeof(arr)) / (sizeof(arr[0])))

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
        *err = MC4_ERR_MAX_TOKENS;
        return;
    } else {
        // MLOG_logf("Added token: %s", token_to_str(&token));
        list->tokens[list->tkns_pos] = token;
        list->tkns_pos++;
    }
}

double read_num(struct StringReader* reader, MC4_ErrorCode* err) {
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

        while (isdigit(reader_get_current(reader)) ||
               (reader_get_current(reader) == '.')) {
            if (reader_get_current(reader) == '.') {
                *err = MC4_ERR_NUM_FMT_ERR;
                return 0;
            } else {
                decimal_part += (reader_get_current(reader) - '0') / divisor;
                divisor *= 10;
                reader_advance(reader);
            }
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

const char* find_const_str(struct StringReader* reader) {
    const char* const CONSTANTS[] = {"pi", "e"};
    const int SIZE = sizeof(CONSTANTS) / sizeof(CONSTANTS[0]);

    for (int i = 0; i < SIZE; i++) {
        if (string_at(CONSTANTS[i], reader->str, reader->pos)) {
            return CONSTANTS[i];
        }
    }
    return NULL;
}

double const_str_to_value(const char* s) {
    if (strcmp(s, "pi") == 0) {
        return M_PI;
    } else if (strcmp(s, "e") == 0) {
        return M_E;
    }

    return 0;
}

/**
 * Advances reader until there is no more whitespace to read.
 */
static void reader_handle_whitespace(struct StringReader* reader) {
    while (isspace(reader_get_current(reader))) {
        reader_advance(reader);
    }
}

/**
 * Tokenizes all sequential operator characters such as '+' and '^'.
 */
static void reader_handle_op(struct StringReader* reader,
                             struct TokensList* list, MC4_ErrorCode* err) {
    char current_ch = reader_get_current(reader);
    while ((strchr("+-*/^", current_ch) != NULL) && (current_ch != '\0')) {
        // MLOG.logf("Received character '%c' in reader_handle_op()",
        // current_ch);
        add_token(list, (struct Token){.type = TYPE_OPERATOR, .op = current_ch},
                  err);
        reader_advance(reader);
        current_ch = reader_get_current(reader);
    }
}

/**
 * When a digit is found, the value wil be read and added to tokens.
 */
static void reader_handle_digit(struct StringReader* reader,
                                struct TokensList* list, MC4_ErrorCode* err) {
    if (isdigit(reader_get_current(reader))) {
        double value = read_num(reader, err);
        add_token(list, (struct Token){.type = TYPE_NUMBER, .value = value},
                  err);
    }
}

/**
 * Tokenizes all sequential parenthesis.
 */
static void reader_handle_par(struct StringReader* reader,
                              struct TokensList* list, MC4_ErrorCode* err) {
    char current_ch = reader_get_current(reader);
    while (current_ch == '(' || current_ch == ')') {
        if (current_ch == '(') {
            add_token(list, (struct Token){.type = TYPE_PAR_LEFT}, err);
        } else {
            add_token(list, (struct Token){.type = TYPE_PAR_RIGHT}, err);
        }
        reader_advance(reader);
        current_ch = reader_get_current(reader);
    }
}

/**
 * When the reader encounters a function, it will tokenize the function and add
 * it to `list.tokens`.
 */
static bool reader_handle_func(struct StringReader* reader,
                               struct TokensList* list, MC4_ErrorCode* err) {
    if (is_func_str(reader)) {
        enum FuncType func_type = funcstr_to_type(reader);
        add_token(list,
                  (struct Token){.type = TYPE_FUNCTION, .func_type = func_type},
                  err);
        return true;
    }
    return false;
}

/**
 * When the reader encounters a constant, it will tokenize the constant and add
 * it to `list.tokens`.
 */
static bool reader_handle_const(struct StringReader* reader,
                                struct TokensList* list, MC4_ErrorCode* err) {
    const char* const_str = find_const_str(reader);
    if (const_str != NULL) {
        double value = const_str_to_value(const_str);
        add_token(list, (struct Token){.type = TYPE_NUMBER, .value = value},
                  err);
        reader->pos += strlen(const_str);
        return true;
    }
    return false;
}

/**
 * When the reader encounters a single letter which isn't part of a function or
 * constant.
 */
static void reader_handle_var(struct StringReader* reader,
                              struct TokensList* list, MC4_ErrorCode* err) {
    if (isalpha(reader_get_current(reader))) {
        add_token(list,
                  (struct Token){.type = TYPE_VARIABLE,
                                 .symbol = reader_get_current(reader)},
                  err);
        reader_advance(reader);
    }
}

/**
 * Takes in string and tokenized it, writing to `list`. An error is
 * written to `err`.
 */
struct TokensList tokenize(const char* equ, MC4_ErrorCode* err) {
    // TODO: add error handling in tokenize().
    struct TokensList tokens_list = new_list();
    const unsigned int EQU_LEN = strlen(equ);
    struct StringReader reader = new_string_reader(equ);

    while (reader.pos < EQU_LEN) {
        reader_handle_whitespace(&reader);
        reader_handle_op(&reader, &tokens_list, err);
        reader_handle_digit(&reader, &tokens_list, err);
        reader_handle_par(&reader, &tokens_list, err);
        if (reader_handle_func(&reader, &tokens_list, err)) continue;
        if (reader_handle_const(&reader, &tokens_list, err)) continue;
        /* It is necessary to continue loop to avoid reading functions or
        constants as variables. */
        reader_handle_var(&reader, &tokens_list, err);
    }

    return tokens_list;
}

static struct MC4_VariableSet new_var_set() {
    return (struct MC4_VariableSet){.exists_hashmap = {false},
                                    .values_hashmap = {0}};
}

// static int letter_to_key(char ch) {
//     if (isupper(ch))
//         return ch - 'A';
//     else if (islower(ch)) {
//         return (ch - 'a') + MC4_VARSET_HALF_SIZE;
//     } else {
//         MLOG.errorf("letter_to_key() - invalid letter: %c", ch);
//         return 0;
//     }
// }

// static char key_to_letter(int key) {
//     if ((key >= 0) && (key < MC4_VARSET_HALF_SIZE)) {
//         return key + 'A';
//     } else if ((key >= MC4_VARSET_HALF_SIZE) && (key < MC4_VARSET_SIZE)) {
//         return key - MC4_VARSET_HALF_SIZE + 'a';
//     } else {
//         MLOG.errorf("key_to_letter() - invalid key: %d", key);
//         return 0;
//     }
// }

// extern void set_var(struct MC4_VariableSet* vars, char var, double value) {
//     int key = letter_to_key(var);
//     vars->exists_hashmap[key] = true;
//     vars->values_hashmap[key] = value;
// }

static void load_vars(struct MC4_Result* result, struct MC4_VariableSet* vars) {
    for (int i = 0; i < MC4_VARSET_SIZE; i++) {
        if (vars->exists_hashmap[i]) {
            set_var(&result->vars, key_to_letter(i), vars->values_hashmap[i]);
        }
    }
}

struct Parser {
    struct Token* tokens;
    unsigned int pos;
    struct MC4_VariableSet* vars;
};

struct Parser new_parser(struct TokensList* list,
                         struct MC4_VariableSet* vars) {
    return (struct Parser){
        .tokens = list->tokens,
        .pos = 0,
        .vars = vars,
    };
}

struct Token* parser_get_current(struct Parser* parser) {
    return &parser->tokens[parser->pos];
}

void parser_consume(struct Parser* parser, enum TokenType type,
                    MC4_ErrorCode* err) {
    struct Token* current = parser_get_current(parser);
    if (current->type == type) {
        parser->pos++;
    } else {
        *err = MC4_ERR_UNEXPECTED_TOKEN;
    }
}

double parse_func(struct Parser* parser, MC4_ErrorCode* err,
                  enum AngleMode angle_mode);
double parse_exp(struct Parser* parser, MC4_ErrorCode* err,
                 enum AngleMode angle_mode);
double parse_multdiv(struct Parser* parser, MC4_ErrorCode* err,
                     enum AngleMode angle_mode);
double parse_addsub(struct Parser* parser, MC4_ErrorCode* err,
                    enum AngleMode angle_mode);
double parse_numpar(struct Parser* parser, MC4_ErrorCode* err,
                    enum AngleMode angle_mode);

/**
 * Preforms the conversion from degrees to radians (if necessary).
 */
double convert_angle_units(double angle, enum AngleMode angle_mode) {
    if (angle_mode == ANGLE_MODE_RAD) {
        return angle;
    } else if (angle_mode == ANGLE_MODE_DEG) {
        return angle * (M_PI / 180);
    } else {
        MLOG.panic("Angle mode should only be a valid state.");
        return 0;
    }
}

double parse_func(struct Parser* parser, MC4_ErrorCode* err,
                  enum AngleMode angle_mode) {
    struct Token* current = parser_get_current(parser);
    if (current->type == TYPE_FUNCTION) {
        parser_consume(parser, TYPE_FUNCTION, err);
        double value = parse_func(parser, err, angle_mode);
        if ((*err) != MC4_ERR_NONE) return 0;
        switch (current->func_type) {
        case FN_SIN: return sin(convert_angle_units(value, angle_mode));
        case FN_COS: return cos(convert_angle_units(value, angle_mode));
        case FN_TAN: return tan(convert_angle_units(value, angle_mode));
        case FN_ASIN: return asin(value);
        case FN_ACOS: return acos(value);
        case FN_ATAN: return atan(value);
        case FN_LOG_10: return log10(value);
        case FN_LOG_E: return log(value);
        }
    } else if ((current->type == TYPE_PAR_LEFT) ||
               (current->type == TYPE_NUMBER) ||
               (current->type == TYPE_VARIABLE)) {
        double value = parse_numpar(parser, err, angle_mode);
        if ((*err) != MC4_ERR_NONE)
            return 0;
        else
            return value;
    } else {
        *err = MC4_ERR_UNEXPECTED_TOKEN;
    }
    return 0;
}

double parse_exp(struct Parser* parser, MC4_ErrorCode* err,
                 enum AngleMode angle_mode) {
    double value = parse_func(parser, err, angle_mode);
    struct Token* current = parser_get_current(parser);
    while (current->type == TYPE_OPERATOR && current->op == '^') {
        parser_consume(parser, TYPE_OPERATOR, err);
        value = pow(value, parse_func(parser, err, angle_mode));
        if ((*err) != MC4_ERR_NONE) return 0;
        current = parser_get_current(parser);
    }
    return value;
}

double parse_multdiv(struct Parser* parser, MC4_ErrorCode* err,
                     enum AngleMode angle_mode) {
    double value = parse_exp(parser, err, angle_mode);
    struct Token* current = parser_get_current(parser);
    while (current->type == TYPE_OPERATOR &&
           ((current->op == '*') || (current->op == '/'))) {
        if (current->op == '*') {
            parser_consume(parser, TYPE_OPERATOR, err);
            value *= parse_exp(parser, err, angle_mode);
        } else {
            parser_consume(parser, TYPE_OPERATOR, err);
            value /= parse_exp(parser, err, angle_mode);
        }
        if ((*err) != MC4_ERR_NONE) return 0;
        current = parser_get_current(parser);
    }
    return value;
}

double parse_addsub(struct Parser* parser, MC4_ErrorCode* err,
                    enum AngleMode angle_mode) {
    double value = parse_multdiv(parser, err, angle_mode);
    struct Token* current = parser_get_current(parser);
    while (current->type == TYPE_OPERATOR &&
           ((current->op == '+') || (current->op == '-'))) {
        if (current->op == '+') {
            parser_consume(parser, TYPE_OPERATOR, err);
            value += parse_multdiv(parser, err, angle_mode);
        } else {
            parser_consume(parser, TYPE_OPERATOR, err);
            value -= parse_multdiv(parser, err, angle_mode);
        }
        if ((*err) != MC4_ERR_NONE) return 0;
        current = parser_get_current(parser);
    }
    return value;
}

double parse_numpar(struct Parser* parser, MC4_ErrorCode* err,
                    enum AngleMode angle_mode) {
    struct Token* current = parser_get_current(parser);
    if (current->type == TYPE_NUMBER) {
        parser_consume(parser, TYPE_NUMBER, err);
        return current->value;
    } else if (current->type == TYPE_VARIABLE) {
        int key = letter_to_key(current->symbol);
        if (parser->vars->exists_hashmap[key]) {
            parser_consume(parser, TYPE_VARIABLE, err);
            return parser->vars->values_hashmap[key];
        } else {
            *err = MC4_ERR_VAR_NOT_FOUND;
            return 0;
        }
    } else if (current->type == TYPE_PAR_LEFT) {
        parser_consume(parser, TYPE_PAR_LEFT, err);
        double value = parse_addsub(parser, err, angle_mode);
        parser_consume(parser, TYPE_PAR_RIGHT, err);
        return value;
    } else {
        *err = MC4_ERR_UNEXPECTED_TOKEN;
        return 0;
    }
}

/**
 * Takes in a list of tokens, and parses the results, returning the result of
 * the expression as a double.
 */
double parse_tokens(struct TokensList* list, struct MC4_VariableSet* vars,
                    MC4_ErrorCode* err, enum AngleMode angle_mode) {
    struct Parser parser = new_parser(list, vars);
    /* Recursive descent parser starts in terms of lowest order of operations.
     */
    double result = parse_addsub(&parser, err, angle_mode);
    return result;
}

/**
 * @brief Evaluates a mathematical expression, returning the result as a double.
 *
 * @param equ equation in the form of a string.
 * @param err if an error occured when evaluating the equation, then it will be
 * writtent to err.
 * @return result
 */
struct MC4_Result MC4_evaluate(const char* equ, struct MC4_VariableSet* vars,
                               struct MC4_Settings* settings) {
    struct MC4_Result result = new_result();
    if (vars != NULL) load_vars(&result, vars);
    MC4_ErrorCode* err = &result.err_code;
    struct TokensList tokens_list = tokenize(equ, err);
    if ((*err) != MC4_ERR_NONE) return result;
    result.value =
        parse_tokens(&tokens_list, &result.vars, err, settings->angle_mode);
    return result;
}
