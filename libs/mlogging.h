#ifndef MLOGGING_LIBRARY_H_
#define MLOGGING_LIBRARY_H_
/** Terminal Colors */

/* Terminal Colors */

struct _internal__mlog_term_colors {
    const char* const Reset;
    const char* const Black;
    const char* const Red;
    const char* const Green;
    const char* const Yellow;
    const char* const Blue;
    const char* const Purple;
    const char* const Cyan;
    const char* const White;
};

static struct _internal__mlog_term_colors MLOG_Color = {
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

/* Function Signatures */
static void _internal__mlog_log(const char* msg);
static void _internal__mlog_logf(const char* msg, ...);
static void _internal__mlog_logc(const char* color, const char* msg);
static void _internal__mlog_logfc(const char* color, const char* msg, ...);
static void _internal__mlog_array_short(short arr[], size_t size);
static void _internal__mlog_array_int(int arr[], size_t size);
static void _internal__mlog_array_long(long arr[], size_t size);
static void _internal__mlog_array_long_long(long long arr[], size_t size);
static void _internal__mlog_array_char(char arr[], size_t size);
static void _internal__mlog_array_double(double arr[], size_t size);
static void _internal__mlog_array_str(char* arr[], size_t size);
static void _internal__mlog_array_custom(void* arr, size_t len,
                                          size_t type_size,
                                          const char* (*fmt_fn)(void*));
static void _internal__mlog_error(const char* msg);
static void _internal__mlog_errorf(const char* msg, ...);
static void _internal__mlog_errorc(const char* color, const char* msg);
static void _internal__mlog_errorfc(const char* color, const char* msg, ...);
static int _internal__mlog_test(const char* tag, int cond);
static void _internal__mlog_test_equ(const char* tag,
                                      int (*equ_test)(void*, void*), void* one,
                                      void* two);
static void _internal__mlog_panic(const char* msg);
static void _internal__mlog_panicf(const char* msg, ...);
static void _internal_mlog_todo(const char* info, const char* file, int line);

/* Configuration */
typedef enum {
    MLOG_CONF_LOG_ON = (1 << 0),  
    MLOG_CONF_ERR_ON = (1 << 1),  
    MLOG_CONF_TEST_ON = (1 << 2), 
    MLOG_CONF_PANIC_ON = (1 << 3),
    MLOG_CONF_ALL_ON = MLOG_CONF_LOG_ON | MLOG_CONF_ERR_ON | MLOG_CONF_TEST_ON | MLOG_CONF_PANIC_ON,
    MLOG_CONF_ALL_OFF = 0,
} MLOG_Config;

static void _internal__mlog_set_config(MLOG_Config config);

struct __internal_mlog_libfuncs {
    MLOG_Config _internal__config;
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
    void (*todo)(const char* info, const char* file, int line);
    /* test */
    int (*test)(const char* tag, int cond);
    void (*test_equ)(const char* tag, int (*equ_test)(void*, void*), void* one,
                     void* two);
    /* config */
    void (*set_config)(MLOG_Config conf);
};

static struct __internal_mlog_libfuncs MLOG = {
    ._internal__config = MLOG_CONF_ALL_ON,
    .log = &_internal__mlog_log,
    .logf = &_internal__mlog_logf,
    .logc = &_internal__mlog_logc,
    .logfc = &_internal__mlog_logfc,
    .char_array = &_internal__mlog_array_char,
    .short_array = &_internal__mlog_array_short,
    .int_array = &_internal__mlog_array_int,
    .long_array = &_internal__mlog_array_long,
    .long_long_array = &_internal__mlog_array_long_long,
    .array_custom = &_internal__mlog_array_custom,
    .error = &_internal__mlog_error,
    .errorf = &_internal__mlog_errorf,
    .errorc = &_internal__mlog_errorc,
    .errorfc = &_internal__mlog_errorfc,
    .panic = &_internal__mlog_panic,
    .panicf = &_internal__mlog_panicf,
    .todo=&_internal_mlog_todo,
    .test = &_internal__mlog_test,
    .test_equ = &_internal__mlog_test_equ,
    .set_config = &_internal__mlog_set_config,
};

static void _internal__mlog_set_config(MLOG_Config config) {
    MLOG._internal__config = config;
}

/* basic logging */

static void _internal__mlog_log(const char* msg) {
    if (MLOG._internal__config & MLOG_CONF_LOG_ON)
        printf("[LOG]: %s\n", msg);
}

static void _internal__mlog_logf(const char* msg, ...) {
    if (MLOG._internal__config & MLOG_CONF_LOG_ON) {
        va_list args;
        va_start(args, msg);
        printf("[LOG]: ");
        vprintf(msg, args);
        puts("");
        va_end(args);
    }
}

static void _internal__mlog_logc(const char* color, const char* msg) {
    if (MLOG._internal__config & MLOG_CONF_LOG_ON)
        printf("%s[LOG]: %s%s\n", color, msg, MLOG_Color.Reset);
}

static void _internal__mlog_logfc(const char* color, const char* msg, ...) {
    if (MLOG._internal__config & MLOG_CONF_LOG_ON) {
        va_list args;
        va_start(args, msg);
        printf("%s[LOG]: ", color);
        vprintf(msg, args);
        puts(MLOG_Color.Reset);
        va_end(args);
    }
}

static void _internal__mlog_array_short(short arr[], size_t size) {
    if (MLOG._internal__config & MLOG_CONF_LOG_ON) {
        printf("[LOG]: [");
        for (size_t i = 0; i < size; i++) {
            printf("%d%s", arr[i], (i == (size - 1) ? "" : ", "));
        }
        puts("]");
    }
}

static void _internal__mlog_array_int(int arr[], size_t size) {
    if (MLOG._internal__config & MLOG_CONF_LOG_ON) {
        printf("[LOG]: [");
        for (size_t i = 0; i < size; i++) {
            printf("%d%s", arr[i], (i == (size - 1) ? "" : ", "));
        }
        puts("]");
    }
}

static void _internal__mlog_array_long(long arr[], size_t size) {
    if (MLOG._internal__config & MLOG_CONF_LOG_ON) {
        printf("[LOG]: [");
        for (size_t i = 0; i < size; i++) {
            printf("%ld%s", arr[i], (i == (size - 1) ? "" : ", "));
        }
        puts("]");
    }
}

static void _internal__mlog_array_long_long(long long arr[], size_t size) {
    if (MLOG._internal__config & MLOG_CONF_LOG_ON) {
        printf("[LOG]: [");
        for (size_t i = 0; i < size; i++) {
            printf("%lld%s", arr[i], (i == (size - 1) ? "" : ", "));
        }
        puts("]");
    }
}

static void _internal__mlog_array_char(char arr[], size_t size) {
    if (MLOG._internal__config & MLOG_CONF_LOG_ON) {
        printf("[LOG]: [");
        for (size_t i = 0; i < size; i++) {
            printf("%c%s", arr[i], (i == (size - 1) ? "" : ", "));
        }
        puts("]");
    }
}

static void _internal__mlog_array_double(double arr[], size_t size) {
    if (MLOG._internal__config & MLOG_CONF_LOG_ON) {
        printf("[LOG]: [");
        for (size_t i = 0; i < size; i++) {
            printf("%lf%s", arr[i], (i == (size - 1) ? "" : ", "));
        }
        puts("]");
    }
}

static void _internal__mlog_array_str(char* arr[], size_t size) {
    if (MLOG._internal__config & MLOG_CONF_LOG_ON) {
        printf("[LOG]: [");
        for (size_t i = 0; i < size; i++) {
            printf("%s%s", arr[i], (i == (size - 1) ? "" : ", "));
        }
        puts("]");
    }
}

static void _internal__mlog_array_custom(void* arr, size_t len,
                                          size_t type_size,
                                          const char* (*fmt_fn)(void*)) {
    if (MLOG._internal__config & MLOG_CONF_LOG_ON) {
        printf("[LOG]: [");
        char* ptr_pos = (char*)arr;
        for (size_t i = 0; i < len; i++) {
            printf("%s%s", fmt_fn(ptr_pos), (i == (len - 1)) ? "" : ", ");
            ptr_pos += type_size;
        }
        puts("]");
    }
}

/* error logging */

static void _internal__mlog_error(const char* msg) {
    if (MLOG._internal__config & MLOG_CONF_ERR_ON)
        fprintf(stderr, "[ERROR]: %s\n", msg);
}

static void _internal__mlog_errorf(const char* msg, ...) {
    if (MLOG._internal__config & MLOG_CONF_ERR_ON) {
        va_list args;
        va_start(args, msg);
        fprintf(stderr, "[ERROR]: ");
        vfprintf(stderr, msg, args);
        puts("");
        va_end(args);
    }
}

static void _internal__mlog_errorc(const char* color, const char* msg) {
    if (MLOG._internal__config & MLOG_CONF_ERR_ON)
        fprintf(stderr, "%s[ERROR]: %s%s\n", color, msg, MLOG_Color.Reset);
}

static void _internal__mlog_errorfc(const char* color, const char* msg, ...) {
    if (MLOG._internal__config & MLOG_CONF_ERR_ON) {
        va_list args;
        va_start(args, msg);
        fprintf(stderr, "%s[ERROR]: ", color);
        vfprintf(stderr, msg, args);
        puts(MLOG_Color.Reset);
        va_end(args);
    }
}

/* testing */
static int _internal__mlog_test(const char* tag, int cond) {
    if (MLOG._internal__config & MLOG_CONF_TEST_ON) {
        if (cond) {
            printf("[TEST] %s: %s[PASSED]%s\n", tag, MLOG_Color.Green,
                   MLOG_Color.Reset);
            return 1;
        } else {
            printf("[TEST] %s: %s[FAILED]%s\n", tag, MLOG_Color.Red,
                   MLOG_Color.Reset);
            return 0;
        }
    }
    return 0;
}

static void _internal__mlog_test_equ(const char* tag,
                                      int (*equ_test)(void*, void*), void* one,
                                      void* two) {
    if (MLOG._internal__config & MLOG_CONF_TEST_ON) {
        if (equ_test(one, two)) {
            printf("[TEST] %s: %s[PASSED]%s\n", tag, MLOG_Color.Green,
                   MLOG_Color.Reset);
        } else {
            printf("[TEST] %s: %s[FAILED]%s\n", tag, MLOG_Color.Red,
                   MLOG_Color.Reset);
        }
    }
}

static void _internal__mlog_panic(const char* msg) {
    if (MLOG._internal__config & MLOG_CONF_PANIC_ON) {
        printf("[PANIC]: %s\n", msg);
        exit(EXIT_FAILURE);
    }
}

static void _internal__mlog_panicf(const char* msg, ...) {
    if (MLOG._internal__config & MLOG_CONF_PANIC_ON) {
        va_list args;
        va_start(args, msg);
        printf("[PANIC]: ");
        vprintf(msg, args);
        puts("");
        va_end(args);
        exit(EXIT_FAILURE);
    }
}

static void _internal_mlog_todo(const char* info, const char* file, int line) {
    if (info != NULL) {
        printf("[TODO] '%s' | Where: %s | Line: %d\n", info, file, line);
    } else {
        printf("[TODO] Unimplemented! Where: %s | Line: %d\n", file, line);
    }
    exit(EXIT_FAILURE);
}

#endif
