#ifndef HELPER_H
#define HELPER_H
#include "Task1.h"

// Forward declaration of the Stack struct to avoid circular dependencies.
// The actual definition should be in its own header file.
// typedef struct Stack Stack; // <-- DELETE THIS LINE

/**
 * @brief Converts a prefix expression string into a stack of tokens.
 * @param prefix_expression The null-terminated string containing the prefix formula.
 * @return A pointer to a new Stack containing the expression tokens, or NULL on failure.
 */
Stack* prefixToStack(const char* prefix_expression);

#endif // HELPER_H