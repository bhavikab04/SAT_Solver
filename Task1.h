#ifndef TASK1_H
#define TASK1_H

/**
 * @file Task1.h
 * @brief Header file for Task 1: Infix to Prefix Conversion.
 *
 * This file defines the structure for a Stack used to hold string
 * tokens and declares the public functions required for stack
 * operations, reading input, and the main conversion logic.
 */

#include <stdio.h> // Needed for FILE* in read_line prototype

// --- Stack Structure Definition ---

/**
 * @brief A stack structure for storing strings (char*).
 *
 * Used as both the operator stack and the output stack during
 * the infix-to-prefix conversion.
 */
typedef struct {
    char** data;    // Array of strings (char*). Each string is dynamically allocated.
    int top;        // Index of the top element (-1 if empty).
    int capacity;   // Max number of items the stack can hold.
} Stack;

// --- Public Function Prototypes ---

// --- Stack functions ---

/**
 * @brief Creates and initializes a new stack.
 * @param capacity The maximum number of items the stack can hold.
 * @return A pointer to the newly allocated Stack, or NULL on failure.
 */
Stack* createStack(int capacity);

/**
 * @brief Checks if the stack is empty.
 * @param stack Pointer to the stack.
 * @return 1 (true) if empty, 0 (false) otherwise.
 */
int isEmpty(Stack* stack);

/**
 * @brief Pushes a string item onto the stack.
 * @note This function creates a copy of the item string.
 * @param stack Pointer to the stack.
 * @param item The string to be pushed.
 */
void push(Stack* stack, const char* item);

/**
 * @brief Pops a string item from the stack.
 * @note The caller is responsible for freeing the returned string.
 * @param stack Pointer to the stack.
 * @return The string from the top of the stack, or NULL if empty.
 */
char* pop(Stack* stack);

/**
 * @brief Frees all memory associated with the stack.
 * This includes all strings still on the stack, the data array,
 * and the stack structure itself.
 * @param stack Pointer to the stack to be freed.
 */
void freeStack(Stack* stack);

// --- Utility function ---

/**
 * @brief Reads a line of arbitrary length from a stream.
 * @param stream The file stream (e.g., stdin).
 * @return A dynamically allocated string containing the line, or NULL on failure.
 * The caller must free this string.
 */
char* read_line(FILE* stream);

// --- Main task function ---

/**
 * @brief Converts an infix expression string to a prefix expression stack.
 * @param infix The null-terminated infix expression string.
 * @return A pointer to a Stack containing the prefix expression tokens.
 * The stack is ordered such that iterating from `top` down to `0`
 * gives the correct prefix expression order.
 * Returns NULL on failure.
 */
Stack* task1_infixToPrefix(const char* infix);

#endif // TASK1_H