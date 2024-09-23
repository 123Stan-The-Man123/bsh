#include <stdbool.h>

/**
 * Compares the input to the available built-in commands.
 * Executes built-ins and returns `true`, otherwise just returns `false`.
 */
bool detect_builtin(char const *args[static 1]);
