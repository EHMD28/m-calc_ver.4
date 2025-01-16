#ifndef MLOGGING_LIBRARY_H_
#define MLOGGING_LIBRARY_H_

/** Terminal Colors */

/* Reset */
#define SET_DEFAULT "\033[0m"
/* Terminal Colors */
#define NO_COLOR SET_DEFAULT
#define TC_BLACK "\033[0;30m"
#define TC_RED "\033[0;31m"
#define TC_GREEN "\033[0;32m"
#define TC_YELLOW "\033[0;33m"
#define TC_BLUE "\033[0;34m"
#define TC_PURPLE "\033[0;35m"
#define TC_CYAN "\033[0;36m"
#define TC_WHITE "\033[0;37m"

    /* Logging Functions */

    #include <stdio.h>
    #include <stdlib.h>
    #include <stdarg.h>

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

    static void __internal__mlog_array_int(int arr[], size_t size) {
        printf("[LOG]: [");
        for (size_t i = 0; i < size; i++) {
            printf("%d%s", arr[i], (i == (size - 1) ? "" : ", "));
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
    static int __internal__mlog_test(int cond, const char* tag) {
        if (cond) {
            printf("[TEST] %s: %s[PASSED]%s\n", tag, TC_GREEN, NO_COLOR);
            return 0;
        } else {
            printf("[TEST] %s: %s[FAILED]%s\n", tag, TC_RED, NO_COLOR);
            return 1;
        }
    }

    static int __internal__mlog_testc(const char* color, int cond, const char* tag) {
        if (cond) {
            printf("%s[TEST] %s: %s[PASSED]%s\n", color, tag, TC_GREEN, NO_COLOR);
            return 0;
        } else {
            printf("%s[TEST] %s: %s[FAILED]%s\n", color, tag, TC_RED, NO_COLOR);
            return 1;
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

    #ifndef MLOGOFF
        /* standard */
        #define MLOG_log(msg) __internal__mlog_log(msg);
        #define MLOG_error(msg) __internal__mlog_error(msg);
        #define MLOG_test(tag, cond) __internal__mlog_test(cond, tag);

        /* arrays */
        #define MLOG_array_int(arr, size) __internal__mlog_array_int(arr, size);
        #define MLOG_array_char(arr, size) __internal__mlog_array_char(arr, size);
        #define MLOG_array_double(arr, size) __internal__mlog_array_double(arr, size);
        #define MLOG_array_str(arr, size) __internal__mlog_array_str(arr, size);
        #define MLOG_array_custom(arr, size, fmt_fn) {\
                    printf("[LOG]: [");\
                    for (int __i = 0; __i < size; __i++) {\
                        fmt_fn(&arr[__i]);\
                        printf((__i == (size - 1)) ? "" : ", " );\
                        fflush(stdout);\
                    };\
                    puts("]");\
                }

        /* format */
        #define MLOG_logf(msg, ...) __internal__mlog_logf(msg, __VA_ARGS__);
        #define MLOG_errorf(msg, ...) __internal__mlog_errorf(msg, __VA_ARGS__);

        /* color */
        #define MLOG_logc(color, msg) __internal__mlog_logc(color, msg);
        #define MLOG_errorc(color, msg) __internal__mlog_errorc(color, msg);
        #define MLOG_testc(tag, color, cond) __internal__mlog_testc(color, cond, tag);

        /* color and format */
        #define MLOG_logfc(color, msg, ...) __internal__mlog_logfc(color, msg, __VA_ARGS__);
        #define MLOG_errorfc(color, msg, ...) __internal__mlog_errorfc(color, msg, __VA_ARGS__);

        /* panic */
        #define MLOG_panic(msg) __internal__mlog_panic(msg);
        #define MLOG_panicf(msg, ...) __internal__mlog_panicf(msg, __VA_ARGS__);
    #else
        /* standard */
        #define MLOG_log(msg) ((void) 0);
        #define MLOG_error(msg) ((void) 0);
        #define MLOG_test(tag, cond) ((void) 0);

        /* arrays */
        #define MLOG_array_int(arr, size) ((void)0);
        #define MLOG_array_char(arr, size) ((void)0);
        #define MLOG_array_double(arr, size) ((void)0);
        #define MLOG_array_str(arr, size) ((void)0);
        #define MLOG_array_custom(arr, size, fmt_fn) ((void)0);

        /* format */
        #define MLOG_logf(msg, ...) ((void)0);
        #define MLOG_errorf(msg, ...) ((void)0);

        /* color */
        #define MLOG_logc(color, msg) ((void)0);
        #define MLOG_errorc(color, msg) ((void)0);
        #define MLOG_testc(color, cond, tag) ((void)0);

        /* color and format */
        #define MLOG_logfc(color, msg, ...) ((void)0);
        #define MLOG_errorfc(color, msg, ...) ((void)0);

        /* panic */
        #define MLOG_panic(msg) ((void)0);
        #define MLOG_panicf(msg, ...) ((void)0);
    #endif
#endif
