#include "arachne_strlib.h"
#include <ctype.h>
#include <stdio.h>
#include <stdlib.h>
#include <string.h>

extern struct ArachneString arachne_new_str(const char* s) {
    return (struct ArachneString){
        .str = s,
        .start = 0,
        .len = 0,
        .buf = NULL,
    };
}

extern struct ArachneString arachne_new_str_ws(const char* s, size_t start) {
    return (struct ArachneString){
        .str = s,
        .start = start,
        .len = 0,
        .buf = NULL,
    };
}

extern struct ArachneString arachne_new_range(const char* s, size_t start,
                                              size_t end) {
    return (struct ArachneString){
        .str = s,
        .start = start,
        .len = (end - start),
        .buf = NULL,
    };
}

extern void arachne_set_str(struct ArachneString* astr, const char* s) {
    astr->str = s;
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
    astr->str = s;
    astr->start = start;
    astr->len = (end - start);
}

extern const char* arachne_get_range(struct ArachneString* astr) {
    if (astr->buf != NULL) free(astr->buf);
    astr->buf = calloc(astr->len + 1, sizeof(char));
    memcpy(astr->buf, &astr->str[astr->start], astr->len);
    return astr->buf;
}

extern void arachne_free(struct ArachneString* astr) {
    free(astr->buf);
}

extern const char* arachne_read_word(struct ArachneString* astr) {
    const size_t STRING_LEN = strlen(astr->str);
    while (isspace(astr->str[astr->start]) || astr->str[astr->start] == '\0') {
        if (astr->start >= STRING_LEN) return NULL;
        astr->start++;
    }
    astr->len = 0;
    while (!isspace(astr->str[astr->start + astr->len])) {
        astr->len++;
    }
    const char* ret = arachne_get_range(astr);
    astr->start += astr->len;
    return ret;
}

extern const char* arachne_read_word_wd(struct ArachneString* astr,
                                        char delimiter) {
    const size_t STRING_LEN = strlen(astr->str);
    if (astr->start >= STRING_LEN) return NULL;
    astr->len = 0;
    if (astr->str[astr->start + astr->len] == delimiter) astr->start++;
    while (astr->str[astr->start + astr->len] != delimiter) {
        if ((astr->start + astr->len) >= STRING_LEN) break;
        astr->len++;
    }
    const char* ret = arachne_get_range(astr);
    astr->start += astr->len;
    return ret;
}
