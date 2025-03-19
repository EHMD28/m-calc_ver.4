#include "cli.h"
#include "../../libs/arachne-strlib/arachne_strlib.h"
#include "../../libs/mlogging.h"
#include "../mcalc4/mcalc4.h"
// #include <ctype.h>
#include <stdio.h>
#include <string.h>

void evaluate_all(const char* equations[], int num_equs) {
    struct MC4_Result result;

    for (int i = 1; i < num_equs; i++) {
        result = MC4_evaluate(equations[i], NULL);

        if (MC4_error_occured(&result)) {
            printf("%s = ERROR\nAn Error occured\n", equations[i]);
            // TODO: implement error handling in main
        } else {
            printf("%s = %lf\n", equations[i], result.value);
        }
    }
}

enum AngleMode {
    DEG,
    RAD,
};

struct MC4_Settings {
    enum AngleMode angle_mode;
};

static struct MC4_Settings settings_default() {
    return (struct MC4_Settings){
        .angle_mode = RAD,
    };
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

static void handle_let_command(ArachneString* astr,
                               struct MC4_VariableSet* varset) {
    const char* var_name_str = arachne_read_word(astr);
    if (var_name_str == NULL)
        MLOG.todo("Variable name was not found.", __FILE__, __LINE__);
    if (strlen(var_name_str) != 1)
        MLOG.todo("Variable name was too long.", __FILE__, __LINE__);
    if (!isalpha(var_name_str[0]))
        MLOG.todo("Variable was not alpha character.", __FILE__, __LINE__);
    const char var_name = var_name_str[0];
    const char* equal_sign = arachne_read_word(astr);
    if (equal_sign == NULL)
        MLOG.todo("Equal sign was not found", __FILE_NAME__, __LINE__);
    if (strcmp(equal_sign, "=") != 0)
        MLOG.todo("Unexpected character. Expected '='", __FILE__, __LINE__);
    const char* expression = arachne_read_rest(astr);
    // TODO: handle errors
    MC4_Result result = MC4_evaluate(expression, varset);
    set_var(varset, var_name, result.value);
    MLOG.logf("Set variable '%c' to %lf", var_name, result.value);
    arachne_free(astr);
}

static void handle_set_command(ArachneString* astr,
                               struct MC4_Settings* settings) {
    (void)astr;
    (void)settings;
}

static void handle_command(enum Command command, ArachneString* astr,
                           struct MC4_VariableSet* varset,
                           struct MC4_Settings* settings) {
    switch (command) {
    case CMD_LET: handle_let_command(astr, varset); break;
    case CMD_SET: handle_set_command(astr, settings); break;
    case CMD_HELP: puts(HELP_STR); break;
    case CMD_QUIT: /* handled elsewere */ break;
    default: break;
    }
}

void start_cli() {
    // TODO: Add tests
    // struct Settings settings = settings_default();
    char buffer[512] = {0};
    (void)HELP_STR;
    struct MC4_VariableSet vars = new_varset();
    struct MC4_Settings settings = settings_default();
    ArachneString astr = arachne_new_str(buffer);

    while (true) {
        printf("(mcalc4) ");
        /* Break when fgets() encounted an EOF. */
        if (fgets(buffer, 512, stdin) == NULL) break;
        arachne_set_str(&astr, buffer);
        const char* command_str = arachne_read_word(&astr);
        enum Command command = str_to_command(command_str);
        if (command == CMD_QUIT) {
            break;
        } else {
            handle_command(command, &astr, &vars, &settings);
            // MLOG.todo(NULL, __FILE__, __LINE__);
        }
    }
}
