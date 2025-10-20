/*
#include <stdio.h>
#include <stdlib.h>
#include <string.h>
#include "helper.h"

// Assume Stack definitions are in another header file, e.g., "Stack.h"
// This would typically include:
// typedef struct Stack Stack;
// Stack* createStack(unsigned capacity);
// void push(Stack* stack, void* item);
// bool isFull(Stack* stack);

/**
 * @brief Converts a prefix expression string into a stack of tokens.
 *
 * This function iterates through the prefix string from right to left,
 * treating each character as a token. Each token is allocated as a new
 * string and pushed onto the stack. This reverse-order push ensures that
 * when the stack is popped, tokens are retrieved in the original
 * left-to-right order of the prefix expression.
 *
 * @param prefix_expression The null-terminated string containing the prefix formula.
 * @return A pointer to a new Stack containing the expression tokens, or NULL on failure.
 */
/*
Stack* prefixToStack(const char* prefix_expression) {
    if (!prefix_expression) {
        fprintf(stderr, "Error: Input prefix expression cannot be NULL.\n");
        return NULL;
    }

    int length = strlen(prefix_expression);
    // Create a stack with enough capacity for all characters.
    // Assuming a 'createStack' function exists from your project setup.
    Stack* stack = createStack(length);

    if (!stack) {
        fprintf(stderr, "Error: Failed to create stack.\n");
        return NULL;
    }

    // Iterate from the end of the string to the beginning.
    for (int i = length - 1; i >= 0; i--) {
        // Allocate memory for the character as a null-terminated string (token).
        // Size is 2: one for the character, one for the '\0'.
        char* token = (char*)malloc(2 * sizeof(char));
        if (!token) {
            fprintf(stderr, "Error: Memory allocation failed for token.\n");
            // Note: In a real-world scenario, you'd need to free the stack
            // and all previously allocated tokens here to prevent memory leaks.
            // For simplicity, we'll exit or return NULL.
            freeStack(stack); // Assuming freeStack handles freeing tokens.
            return NULL;
        }

        // Create the string token.
        token[0] = prefix_expression[i];
        token[1] = '\0';

        // Push the new token onto the stack.
        // Assuming a 'push' function that takes a void* or char*.
        push(stack, token);
    }

    return stack;
}

*/