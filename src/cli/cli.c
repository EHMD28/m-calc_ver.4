#include "cli.h"
#include "../../libs/arachne-strlib/arachne_strlib.h"
#include "../../libs/mlogging.h"
#include "../mcalc4/mcalc4.h"
#include "cli_types.h"
// #include <ctype.h>
#include <stdio.h>
#include <string.h>

void evaluate_all(const char* equations[], int num_equs) {
    struct MC4_Result result;

    for (int i = 1; i < num_equs; i++) {
        result = MC4_evaluate(equations[i], NULL, NULL);

        if (MC4_error_occured(&result)) {
            printf("%s = ERROR\nAn Error occured\n", equations[i]);
            // TODO: implement error handling in main
        } else {
            printf("%s = %lf\n", equations[i], result.value);
        }
    }
}

static bool str_is_empty(const char* s) {
    const size_t STRING_LENGTH = strlen(s);
    for (size_t i = 0; i < STRING_LENGTH; i++) {
        if (!isspace(s[i])) return false;
    }
    return true;
}

static void trim_str_end(char* s) {
    const size_t STRING_LENGTH = strlen(s);
    for (int i = STRING_LENGTH - 1; (i >= 0) && isspace(s[i]); i--) {
        s[i] = '\0';
    }
}

static enum SetttingName str_to_setting_name(const char* s) {
    if (strcasecmp("angle", s) == 0) {
        return SETNAME_ANGLE_MODE;
    } else {
        return SETNAME_UNKOWN;
    }
}

static const char* const HELP_STR =
    "\nExpressions - Evaluate a mathematical expression. Basic arithmetic\n"
    "operators (+, -, *, /, and ^) are supported as well as trigonometric\n"
    "functions(such sin and arctan), logarithms(log and ln), and\n"
    "constants(e and pi)\n\n"
    "Variables - Syntax: `let{variable} = {value}`. Set a variable with\n"
    "name {variable} to {value} {Value can be} any valid expression.\n\n"
    "Settings - Syntax: `set{setting_name} { value }`. There are a\n"
    "few settings in M-Calculator 4 which can be adjusted: ANGLE_MODE,\n"
    "{TBD}...\n";

enum Command {
    CMD_LET,
    CMD_SET,
    CMD_HELP,
    CMD_QUIT,
    CMD_NONE,
};

static const char* command_to_str(enum Command command) {
    switch (command) {
    case CMD_LET: return "CMD_LET";
    case CMD_SET: return "CMD_SET";
    case CMD_HELP: return "CMD_HELP";
    case CMD_QUIT: return "CMD_QUIT";
    case CMD_NONE: return "CMD_NONE";
    default: return NULL;
    }
}

static enum Command str_to_command(const char* s) {
    if (strcasecmp(s, "let") == 0) {
        return CMD_LET;
    } else if (strcasecmp(s, "set") == 0) {
        return CMD_SET;
    } else if (strcasecmp(s, "help") == 0) {
        return CMD_HELP;
    } else if ((strcasecmp(s, "quit") == 0) || (strcasecmp(s, "exit") == 0)) {
        return CMD_QUIT;
    } else {
        return CMD_NONE;
    }
}

enum CommandParseError {
    CPE_NO_ERROR,
    /* Let Command */
    CPE_NO_VAR_NAME,
    CPE_VAR_NAME_TOO_LONG,
    CPE_VAR_NAME_NOT_ALPHA,
    CPE_EQUAL_SIGN_NOT_FOUND,
    CPE_EXPECTED_EQUAL_SIGN,
    CPE_EXPECTED_EXPRESSION,
    /* Set Command */
    CPE_UNKOWN_SETTING,
    CPE_EXPECTED_SET_VALUE,
    CPE_INVALID_SET_VALUE,
};

static void print_syntax_error(const char* info) {
    printf("Syntax Error: %s.\n", info);
}

static enum CommandParseError
handle_let_command(ArachneString* astr, struct MC4_VariableSet* varset,
                   struct MC4_Settings* settings) {
    const char* var_name_str = arachne_read_word(astr);
    if (var_name_str == NULL) return CPE_NO_VAR_NAME;
    if (strlen(var_name_str) != 1) return CPE_VAR_NAME_TOO_LONG;
    if (!isalpha(var_name_str[0])) return CPE_VAR_NAME_NOT_ALPHA;
    const char var_name = var_name_str[0];
    const char* equal_sign = arachne_read_word(astr);
    if (equal_sign == NULL) return CPE_EQUAL_SIGN_NOT_FOUND;
    if (strcmp(equal_sign, "=") != 0) return CPE_EXPECTED_EQUAL_SIGN;
    const char* expression = arachne_read_rest(astr);
    if (str_is_empty(expression)) return CPE_EXPECTED_EXPRESSION;
    MC4_Result result = MC4_evaluate(expression, varset, settings);
    set_var(varset, var_name, result.value);
    MLOG.logf("Set variable '%c' to %lf", var_name, result.value);
    arachne_free(astr);
    return CPE_NO_ERROR;
}

static void handle_let_command_error(enum CommandParseError error) {
    switch (error) {
    case CPE_NO_VAR_NAME: print_syntax_error("Expected variable name"); break;
    case CPE_VAR_NAME_TOO_LONG:
        print_syntax_error("Variable name must be one character");
        break;
    case CPE_VAR_NAME_NOT_ALPHA:
        print_syntax_error("Variable name must be a letter");
        break;
    case CPE_EQUAL_SIGN_NOT_FOUND:
        print_syntax_error("Expected an equal sign");
        break;
    case CPE_EXPECTED_EQUAL_SIGN:
        print_syntax_error("Expected an equal sign");
        break;
    case CPE_EXPECTED_EXPRESSION: print_syntax_error("Expected value"); break;
    default: break;
    }
}

static enum CommandParseError
handle_set_command(ArachneString* astr, struct MC4_Settings* settings) {
    const char* SETTING_NAME = arachne_read_word(astr);
    enum SetttingName setting_name = str_to_setting_name(SETTING_NAME);
    if (setting_name == SETNAME_UNKOWN) return CPE_UNKOWN_SETTING;
    switch (setting_name) {
    case SETNAME_ANGLE_MODE:
        {
            const char* VALUE = arachne_read_word(astr);
            if (VALUE == NULL) return CPE_EXPECTED_SET_VALUE;
            if (strcasecmp("rad", VALUE) == 0) {
                settings->angle_mode = ANGLE_MODE_RAD;
                MLOG.log("Set angle mode to radians.");
            } else if (strcasecmp("deg", VALUE) == 0) {
                settings->angle_mode = ANGLE_MODE_DEG;
                MLOG.log("Set angle mode to degrees");
            } else {
                return CPE_INVALID_SET_VALUE;
            }
        };
    default: break;
    }
    return CPE_NO_ERROR;
}

static void handle_set_command_error(enum CommandParseError error) {
    switch (error) {
    case CPE_UNKOWN_SETTING: print_syntax_error("Unkown setting.");
    case CPE_EXPECTED_SET_VALUE: print_syntax_error("Expected value"); break;
    case CPE_INVALID_SET_VALUE:
        print_syntax_error("Invalid value for setting.");
        break;
    default: break;
    }
}

static void handle_command(enum Command command, ArachneString* astr,
                           struct MC4_VariableSet* varset,
                           struct MC4_Settings* settings) {
    switch (command) {
    case CMD_LET:
        handle_let_command_error(handle_let_command(astr, varset, settings));
        break;
    case CMD_SET:
        handle_set_command_error(handle_set_command(astr, settings));
        break;
    case CMD_HELP: puts(HELP_STR); break;
    case CMD_QUIT: /* handled elsewere */ break;
    default: /* expressions. handled elsewhere. */ break;
    }
}

void start_cli() {
    // TODO: Add tests
    char buffer[512] = {0};
    struct MC4_VariableSet varset = new_varset();
    struct MC4_Settings settings = settings_default();
    ArachneString astr = arachne_new_str(buffer);
    while (true) {
        printf("(mcalc4) ");
        /* Break when fgets() encounters an EOF. */
        if (fgets(buffer, 512, stdin) == NULL) break;
        arachne_set_str(&astr, buffer);
        const char* command_str = arachne_read_word(&astr);
        if (command_str == NULL) continue;
        enum Command command = str_to_command(command_str);
        if (command == CMD_NONE) {
            /* Interperet input as expression. */
            trim_str_end(buffer);
            MC4_Result result = MC4_evaluate(buffer, &varset, &settings);
            if (MC4_error_occured(&result)) {
                print_syntax_error(MC4_get_error_str(&result));
            } else {
                printf("%s = %lf\n", buffer, result.value);
            }
        } else if (command == CMD_QUIT) {
            break;
        } else {
            handle_command(command, &astr, &varset, &settings);
        }
    }
}
