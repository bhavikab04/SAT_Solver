#ifndef TASK1_H
#define TASK1_H


#include <stdio.h> // Needed for FILE* in read_line prototype

// --- Stack Structure Definition ---
typedef struct {
    char** data;    // Array of strings (char*)
    int top;        // Index of the top element
    int capacity;   // Max size of the stack
} Stack;

// --- Public Function Prototypes ---

// Stack functions
Stack* createStack(int capacity);
int isEmpty(Stack* stack);
void push(Stack* stack, const char* item);
char* pop(Stack* stack);
void freeStack(Stack* stack);

// Utility function (used by main to read the CNF output)
char* read_line(FILE* stream);

// Main task function
Stack* task1_infixToPrefix(const char* infix);

#endif // TASK1_H