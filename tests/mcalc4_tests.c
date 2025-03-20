#include "../src/mcalc4/mcalc4.h"
#include "../src/mcalc4/mcalc4_types.h"
#include <float.h>
#include <math.h>

#define ARR_SIZE(arr) ((sizeof(arr)) / (sizeof(arr[0])))

const char* token_type_to_str(enum TokenType type) {
    switch (type) {
    case TYPE_EMPTY: return "EMPTY";
    case TYPE_NUMBER: return "NUMBER";
    case TYPE_OPERATOR: return "OPERATOR";
    case TYPE_PAR_LEFT: return "PAR_LEFT";
    case TYPE_PAR_RIGHT: return "PAR_RIGHT";
    case TYPE_FUNCTION: return "FUNCTION";
    case TYPE_VARIABLE: return "VARIABLE";
    default: return NULL;
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
    return NULL;
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
    case TYPE_FUNCTION: type = "FUNCTION"; break;
    case TYPE_VARIABLE: type = "VARIABLE"; break;
    case TYPE_CONSTANT: type = "CONSTANT"; break;
    }

    if (token->type == TYPE_NUMBER) {
        snprintf(buffer, 100, "%s(%lf)", type, token->value);
    } else if (token->type == TYPE_OPERATOR) {
        snprintf(buffer, 100, "%s(%c)", type, token->op);
    } else if (token->type == TYPE_FUNCTION) {
        snprintf(buffer, 100, "%s(%s)", type,
                 functype_to_str(token->func_type));
    } else if (token->type == TYPE_VARIABLE) {
        snprintf(buffer, 100, "%s(%c)", type, token->symbol);
    } else {
        snprintf(buffer, 100, "%s", type);
    }

    return buffer;
}

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
    case TYPE_FUNCTION:
        return (b.type == TYPE_FUNCTION) && (a.func_type == b.func_type);
    case TYPE_VARIABLE:
        return (b.type == TYPE_VARIABLE) && (a.symbol == b.symbol);
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
        (struct Token){.type = TYPE_NUMBER, .value = M_PI},
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
        (struct Token){.type = TYPE_NUMBER, .value = M_E},
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

static void test_tokenization_six(void) {
    struct Token test[] = {
        (struct Token){.type = TYPE_NUMBER, .value = 2},
        (struct Token){.type = TYPE_OPERATOR, .op = '*'},
        (struct Token){.type = TYPE_VARIABLE, .symbol = 'x'},
        (struct Token){.type = TYPE_OPERATOR, .op = '+'},
        (struct Token){.type = TYPE_NUMBER, .value = 5},
        (struct Token){.type = TYPE_OPERATOR, .op = '*'},
        (struct Token){.type = TYPE_VARIABLE, .symbol = 'y'},
        (struct Token){.type = TYPE_OPERATOR, .op = '+'},
        (struct Token){.type = TYPE_NUMBER, .value = 3},
        (struct Token){.type = TYPE_OPERATOR, .op = '*'},
        (struct Token){.type = TYPE_VARIABLE, .symbol = 'z'},
        (struct Token){.type = TYPE_OPERATOR, .op = '^'},
        (struct Token){.type = TYPE_NUMBER, .value = 2},
    };
    struct TokensList result = tokenize("2*x + 5*y + 3 * z^2", NULL);
    int passed =
        MLOG.test("2*x + 5*y + 3* z^2",
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
    test_tokenization_six();
}

static void run_parse_test(const char* equ, double expected,
                           struct MC4_VariableSet* vars) {
    struct MC4_Settings settings = settings_default();
    struct MC4_Result test = MC4_evaluate(equ, vars, &settings);
    int passed = MLOG.test(equ, doubles_mostly_equal(test.value, expected));
    if (!passed) {
        MLOG.logf("Expected value: %lf | Found value: %lf", expected,
                  test.value);
    }
}

void test_parsing(void) {
    MLOG.log("Parsing Test Suite");
    run_parse_test("2+4", 6, NULL);
    run_parse_test("(2*4/6)^8", 9.98872123151958, NULL);
    run_parse_test("cos(arctan(sin(pi/2)))", 0.7071067811865476, NULL);
    run_parse_test("ln(e^2) + log(10)", 3, NULL);
    struct MC4_VariableSet vars = new_varset();
    set_var(&vars, 'x', 2);
    set_var(&vars, 'y', 3);
    set_var(&vars, 'z', 4);
    run_parse_test("2*x + 5*y + 3 * z^2", 67, &vars);
}
