#include "mcalc4.h"
#include "mlogging.h"
#include <stddef.h>

double MC4_evaluate(const char *equ, MC4_ErrorCode *err) {
    MC4_ErrorCode error_code = NO_ERROR;
    (void) equ;

    if (err != NULL) {
        *err = error_code;
    }

    return 0.0;
}
