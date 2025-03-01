#ifndef MLOGGING_LIBRARY_H_
#define MLOGGING_LIBRARY_H_
/** Terminal Colors */

/* Terminal Colors */
// typedef const char* const char*;
static const char* NO_COLOR = "\033[0m";
// static const char* TC_BLACK = "\033[0;30m";
static const char* TC_RED = "\033[0;31m";
static const char* TC_GREEN = "\033[0;32m";
// static const char* TC_YELLOW = "\033[0;33m";
// static const char* TC_BLUE = "\033[0;34m";
// static const char* TC_PURPLE = "\033[0;35m";
// static const char* TC_CYAN = "\033[0;36m";
// static const char* TC_WHITE = "\033[0;37m";

struct __internal__mlog_term_colors {
    const char* Reset;
    const char* Black;
    const char* Red;
    const char* Green;
    const char* Yellow;
    const char* Blue;
    const char* Purple;
    const char* Cyan;
    const char* White;
};

struct __internal__mlog_term_colors MLOG_Color = {
    .Reset = "\033[0m",
    .Black = "\033[0;30m",
    .Red = "\033[0;31m",
    .Green = "\033[0;32m",
    .Yellow = "\033[0;33m",
    .Blue = "\033[0;34m",
    .Purple = "\033[0;35m",
    .Cyan = "\033[0;36m",
    .White = "\033[0;37m",
};

/* Logging Functions */

#include <stdarg.h>
#include <stdint.h>
#include <stdio.h>
#include <stdlib.h>

/* basic logging */

static void __internal__mlog_log(const char* msg) {
    printf("[LOG]: %s\n", msg);
}

static void __internal__mlog_logf(const char* msg, ...) {
    va_list args;
    va_start(args, msg);
    printf("[LOG]: ");
    vprintf(msg, args);
    puts("");
    va_end(args);
}

static void __internal__mlog_logc(const char* color, const char* msg) {
    printf("%s[LOG]: %s%s\n", color, msg, NO_COLOR);
}

static void __internal__mlog_logfc(const char* color, const char* msg, ...) {
    va_list args;
    va_start(args, msg);
    printf("%s[LOG]: ", color);
    vprintf(msg, args);
    puts(NO_COLOR);
    va_end(args);
}

static void __internal__mlog_array_short(short arr[], size_t size) {
    printf("[LOG]: [");
    for (size_t i = 0; i < size; i++) {
        printf("%d%s", arr[i], (i == (size - 1) ? "" : ", "));
    }
    puts("]");
}

static void __internal__mlog_array_int(int arr[], size_t size) {
    printf("[LOG]: [");
    for (size_t i = 0; i < size; i++) {
        printf("%d%s", arr[i], (i == (size - 1) ? "" : ", "));
    }
    puts("]");
}

static void __internal__mlog_array_long(long arr[], size_t size) {
    printf("[LOG]: [");
    for (size_t i = 0; i < size; i++) {
        printf("%ld%s", arr[i], (i == (size - 1) ? "" : ", "));
    }
    puts("]");
}

static void __internal__mlog_array_long_long(long long arr[], size_t size) {
    printf("[LOG]: [");
    for (size_t i = 0; i < size; i++) {
        printf("%lld%s", arr[i], (i == (size - 1) ? "" : ", "));
    }
    puts("]");
}

static void __internal__mlog_array_char(char arr[], size_t size) {
    printf("[LOG]: [");
    for (size_t i = 0; i < size; i++) {
        printf("%c%s", arr[i], (i == (size - 1) ? "" : ", "));
    }
    puts("]");
}

static void __internal__mlog_array_double(double arr[], size_t size) {
    printf("[LOG]: [");
    for (size_t i = 0; i < size; i++) {
        printf("%lf%s", arr[i], (i == (size - 1) ? "" : ", "));
    }
    puts("]");
}

static void __internal__mlog_array_str(char* arr[], size_t size) {
    printf("[LOG]: [");
    for (size_t i = 0; i < size; i++) {
        printf("%s%s", arr[i], (i == (size - 1) ? "" : ", "));
    }
    puts("]");
}

static void __internal__mlog_array_custom(void* arr, size_t len,
                                          size_t type_size,
                                          const char* (*fmt_fn)(void*)) {
    printf("[LOG]: [");
    char* ptr_pos = (char*)arr;
    for (size_t i = 0; i < len; i++) {
        printf("%s%s", fmt_fn(ptr_pos), (i == (len - 1)) ? "" : ", ");
        ptr_pos += type_size;
    }
    puts("]");
}

/* error logging */

static void __internal__mlog_error(const char* msg) {
    fprintf(stderr, "[ERROR]: %s\n", msg);
}

static void __internal__mlog_errorf(const char* msg, ...) {
    va_list args;
    va_start(args, msg);
    fprintf(stderr, "[ERROR]: ");
    vfprintf(stderr, msg, args);
    puts("");
    va_end(args);
}

static void __internal__mlog_errorc(const char* color, const char* msg) {
    fprintf(stderr, "%s[ERROR]: %s%s\n", color, msg, NO_COLOR);
}

static void __internal__mlog_errorfc(const char* color, const char* msg, ...) {
    va_list args;
    va_start(args, msg);
    fprintf(stderr, "%s[ERROR]: ", color);
    vfprintf(stderr, msg, args);
    puts(NO_COLOR);
    va_end(args);
}

/* testing */
static int __internal__mlog_test(const char* tag, int cond) {
    if (cond) {
        printf("[TEST] %s: %s[PASSED]%s\n", tag, TC_GREEN, NO_COLOR);
        return 1;
    } else {
        printf("[TEST] %s: %s[FAILED]%s\n", tag, TC_RED, NO_COLOR);
        return 0;
    }
}

static void __internal__mlog_test_equ(const char* tag,
                                      int (*equ_test)(void*, void*), void* one,
                                      void* two) {
    if (equ_test(one, two)) {
        printf("[TEST] %s: %s[PASSED]%s\n", tag, TC_GREEN, NO_COLOR);
    } else {
        printf("[TEST] %s: %s[FAILED]%s\n", tag, TC_RED, NO_COLOR);
    }
}

static void __internal__mlog_panic(const char* msg) {
    printf("[PANIC]: %s\n", msg);
    exit(EXIT_FAILURE);
}

static void __internal__mlog_panicf(const char* msg, ...) {
    va_list args;
    va_start(args, msg);
    printf("[PANIC]: ");
    vprintf(msg, args);
    puts("");
    va_end(args);
    exit(EXIT_FAILURE);
}

struct __internal_mlog_libfuncs {
    /* basic */
    void (*log)(const char* msg);
    void (*logf)(const char* msg, ...);
    void (*logc)(const char* color, const char* msg);
    void (*logfc)(const char* color, const char* msg, ...);
    /* array */
    void (*char_array)(char arr[], size_t len);
    void (*short_array)(short arr[], size_t len);
    void (*int_array)(int arr[], size_t len);
    void (*long_array)(long arr[], size_t len);
    void (*long_long_array)(long long arr[], size_t len);
    void (*array_custom)(void* arr, size_t len, size_t type_size,
                         const char* (*to_str)(void*));
    /* error */
    void (*error)(const char* msg);
    void (*errorf)(const char* msg, ...);
    void (*errorc)(const char* color, const char* msg);
    void (*errorfc)(const char* color, const char* msg, ...);
    /* panic */
    void (*panic)(const char* msg);
    void (*panicf)(const char* msg, ...);
    /* test */
    int (*test)(const char* tag, int cond);
    void (*test_equ)(const char* tag, int (*equ_test)(void*, void*), void* one,
                     void* two);
};

static const struct __internal_mlog_libfuncs MLOG = {
    .log = &__internal__mlog_log,
    .logf = &__internal__mlog_logf,
    .logc = &__internal__mlog_logc,
    .logfc = &__internal__mlog_logfc,
    .char_array = &__internal__mlog_array_char,
    .short_array = &__internal__mlog_array_short,
    .int_array = &__internal__mlog_array_int,
    .long_array = &__internal__mlog_array_long,
    .long_long_array = &__internal__mlog_array_long_long,
    .array_custom = &__internal__mlog_array_custom,
    .error = &__internal__mlog_error,
    .errorf = &__internal__mlog_errorf,
    .errorc = &__internal__mlog_errorc,
    .errorfc = &__internal__mlog_errorfc,
    .panic = &__internal__mlog_panic,
    .panicf = &__internal__mlog_panicf,
    .test = &__internal__mlog_test,
    .test_equ = &__internal__mlog_test_equ};

#endif
