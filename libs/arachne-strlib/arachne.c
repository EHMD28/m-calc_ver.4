#include "arachne_strlib.h"
#include <ctype.h>
#include <stdio.h>
#include <stdlib.h>
#include <string.h>

extern struct ArachneString arachne_new_str(const char* s) {
    return (struct ArachneString){
        .src = s,
        .start = 0,
        .len = 0,
        .buf = NULL,
    };
}

extern struct ArachneString arachne_new_str_ws(const char* s, size_t start) {
    return (struct ArachneString){
        .src = s,
        .start = start,
        .len = 0,
        .buf = NULL,
    };
}

extern struct ArachneString arachne_new_range(const char* s, size_t start,
                                              size_t end) {
    return (struct ArachneString){
        .src = s,
        .start = start,
        .len = (end - start),
        .buf = NULL,
    };
}

extern void arachne_set_str(struct ArachneString* astr, const char* s) {
    astr->src = s;
    astr->start = 0;
    astr->len = 0;
}

extern void arachne_set_range(struct ArachneString* astr, size_t start,
                              size_t end) {
    astr->start = start;
    astr->len = (end - start);
}

extern void arachne_set_str_range(struct ArachneString* astr, const char* s,
                                  size_t start, size_t end) {
    astr->src = s;
    astr->start = start;
    astr->len = (end - start);
}

static char get_char_at_start(struct ArachneString* astr) {
    return astr->src[astr->start];
}

static char get_current_char(struct ArachneString* astr) {
    return astr->src[astr->start + astr->len];
}

static size_t get_true_pos(struct ArachneString* astr) {
    return (astr->start + astr->len);
}

extern const char* arachne_get_range(struct ArachneString* astr) {
    if (astr->buf != NULL) free(astr->buf);
    astr->buf = calloc(astr->len + 1, sizeof(char));
    memcpy(astr->buf, &astr->src[astr->start], astr->len);
    return astr->buf;
}

extern void arachne_free(struct ArachneString* astr) {
    free(astr->buf);
}

extern const char* arachne_read_chars(struct ArachneString* astr,
                                      size_t num_chars) {
    const size_t STRING_LEN = strlen(astr->src);
    while ((get_true_pos(astr) < STRING_LEN) && (astr->len < num_chars)) {
        astr->len++;
    }
    const char* ret = arachne_get_range(astr);
    astr->start += astr->len;
    return ret;
}

extern const char* arachne_read_word(struct ArachneString* astr) {
    const size_t STRING_LEN = strlen(astr->src);
    while (isspace(get_char_at_start(astr)) ||
           (get_char_at_start(astr) == '\0')) {
        if (astr->start >= STRING_LEN) return NULL;
        astr->start++;
    }
    astr->len = 0;
    while (!isspace(get_current_char(astr)) &&
           (get_current_char(astr) != '\0')) {
        astr->len++;
    }
    const char* ret = arachne_get_range(astr);
    astr->start += astr->len;
    return ret;
}

extern const char* arachne_read_word_wd(struct ArachneString* astr,
                                        char delimiter) {
    const size_t STRING_LEN = strlen(astr->src);
    if (astr->start >= STRING_LEN) return NULL;
    astr->len = 0;
    if (get_current_char(astr) == delimiter) astr->start++;
    while (get_current_char(astr) != delimiter) {
        if (get_true_pos(astr) >= STRING_LEN) break;
        astr->len++;
    }
    const char* ret = arachne_get_range(astr);
    astr->start += astr->len;
    return ret;
}

extern const char* arachne_read_rest(struct ArachneString* astr) {
    const size_t STRING_LEN = strlen(astr->src);
    astr->len = STRING_LEN - astr->start;
    const char* ret = arachne_get_range(astr);
    astr->start += astr->len;
    return ret;
}
