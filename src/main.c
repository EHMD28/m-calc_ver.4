#include "cli/cli.h"

int main(const int argc, const char* argv[]) {
    /* if `mcacl4` has command_line arguments */
    if (argc > 1) {
        evaluate_all(argv, argc);
    } else {
        start_cli();
    }
}
