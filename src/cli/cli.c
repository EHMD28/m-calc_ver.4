#include "cli.h"
// #include "../libs/mlogging.h"
#include "../mcalc4/mcalc4.h"
#include <ctype.h>
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

static void trim_str_end(char* s, const size_t len) {
    for (int i = len - 1; isspace(s[i]) && (i > 0); i--)
        s[i] = '\0';
}

static void clear_buffer(char* buf, const size_t size) {
    for (size_t i = 0; i < size; i++) {
        buf[i] = '\0';
    }
}

struct WordReader {
    const char* src;
    char buffer[100];
    size_t pos;
};

struct WordReader new_word_reader() {
    return (struct WordReader){
        .src = NULL,
        .buffer = {0},
        .pos = 0,
    };
}

static void word_reader_set_src(struct WordReader* reader, const char* s) {
    clear_buffer(reader->buffer, reader->pos);
    reader->pos = 0;
    reader->src = s;
}

static void word_reader_read_next_word(struct WordReader* reader) {
    clear_buffer(reader->buffer, strlen(reader->buffer));
    const int SRC_LEN = strlen(reader->src);
    for (int i = 0; !isspace(reader->src[reader->pos]) && (i <= SRC_LEN); i++) {
        reader->buffer[i] = reader->src[reader->pos];
        reader->pos++;
    }
}

static const char* word_reader_get_word(struct WordReader* reader) {
    return reader->buffer;
}

enum Command {
    CMD_LET,
    CMD_SET,
    CMD_HELP,
    CMD_INVALID,
};

static enum Command str_to_cmd(const char* s) {
    if (strcmp(s, "let") == 0) {
        return CMD_LET;
    } else if (strcmp(s, "set") == 0) {
        return CMD_SET;
    } else if (strcmp(s, "help") == 0) {
        return CMD_HELP;
    } else {
        return CMD_INVALID;
    }
}

static void parse_let_command(struct WordReader* reader) {
    puts("1");
    (void)reader;
}

static void parse_set_command(struct WordReader* reader) {
    puts("2");
    (void)reader;
}

static void parse_help_command(struct WordReader* reader) {
    (void)reader;
    puts(
        "\nExpressions - Evaluate a mathematical expression. Basic arithmetic\n"
        "operators (+, -, *, /, and ^) are supported as well as trigonometric\n"
        "functions (such sin and arctan), logarithms (log and ln), and\n"
        "constants (e and pi)\n\n"
        "Variables - Syntax: `let {variable} = {value}`. Set a variable with\n"
        "name {variable} to {value}. {Value can be} any valid expression.\n\n"
        "Settings - Syntax: `set {setting_name} {value}`. There are a\n"
        "few ssettings in M-Calculator 4 which can be adjusted: ANGLE_MODE,\n"
        "{TBD}...\n");
}

#define BUFFER_SIZE 512
#define FIRST_WORD_BUF_SIZE 100

void start_cli() {
    bool has_exited = false;
    char buffer[BUFFER_SIZE] = {0};
    struct WordReader word_reader = new_word_reader();
    MC4_Result result;

    while (!has_exited) {
        clear_buffer(buffer, BUFFER_SIZE);
        printf("mcalc4> ");
        if (fgets(buffer, BUFFER_SIZE, stdin) == NULL) {
            has_exited = true;
            continue;
        }
        trim_str_end(buffer, strlen(buffer));
        word_reader_set_src(&word_reader, buffer);
        word_reader_read_next_word(&word_reader);
        // puts(word_reader_get_word(&word_reader));
        enum Command command = str_to_cmd(word_reader_get_word(&word_reader));
        switch (command) {
        case CMD_LET: parse_let_command(&word_reader); break;
        case CMD_SET: parse_set_command(&word_reader); break;
        case CMD_HELP: parse_help_command(&word_reader); break;
        case CMD_INVALID:
            {
                result = MC4_evaluate(buffer, NULL);
                if (MC4_error_occured(&result)) {
                    printf("%s = ERROR\n", buffer);
                } else {
                    printf("%s = %lf\n", buffer, result.value);
                }
            }
            break;
        default: puts("Unrecognized command");
        }
    }
}
