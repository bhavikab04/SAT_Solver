/**
 * @file task1_corrected.c
 * @brief Corrected implementation for infix to prefix conversion.
 *
 * The primary error in the original code was the failure to handle
 * operator precedence. This corrected version includes a precedence()
 * helper function and updates the main conversion loop to correctly
 * manage the operator stack according to standard precedence rules
 * for a right-to-left scan.
 */

#include "Task1.h" // Includes the Stack definition

#include <stdio.h>
#include <stdlib.h>
#include <string.h>
#include <ctype.h>
#include <stdbool.h> // For bool type

// --- Private Helper Function (for portability) ---
static char* my_strrev(char* str) {
    if (!str || !*str) return str;
    int i = 0;
    int j = strlen(str) - 1;
    char temp;
    while (i < j) {
        temp = str[i];
        str[i] = str[j];
        str[j] = temp;
        i++;
        j--;
    }
    return str;
}

// --- Stack Function Implementations ---
Stack *createStack(int capacity) {
    Stack *stack = (Stack *)malloc(sizeof(Stack));
    if (!stack) return NULL;
    stack->data = (char **)malloc(sizeof(char *) * capacity);
    if (!stack->data) {
        free(stack);
        return NULL;
    }
    stack->capacity = capacity;
    stack->top = -1;
    return stack;
}

int isEmpty(Stack *stack) { return stack->top == -1; }

void push(Stack *stack, const char *item) {
    if (stack->top >= stack->capacity - 1) {
        fprintf(stderr, "Stack overflow error\n");
        return;
    }
    stack->data[++stack->top] = strdup(item);
    if (stack->data[stack->top] == NULL) {
        fprintf(stderr, "Memory allocation failed for push\n");
        stack->top--;
    }
}

char *pop(Stack *stack) {
    if (isEmpty(stack)) {
        return NULL;
    }
    return stack->data[stack->top--];
}

void freeStack(Stack *stack) {
    if (stack) {
        while (!isEmpty(stack)) {
            free(pop(stack));
        }
        free(stack->data);
        free(stack);
    }
}
// --- End of Stack Implementation ---


// --- Private Helper Functions ---
static int isOperator(char ch) {
    return ch == '~' || ch == '*' || ch == '+' || ch == '>';
}

static int isOperandStart(char ch) {
    return (ch >= 'a' && ch <= 'z') || (ch >= 'A' && ch <= 'Z');
}

/**
 * @brief Determines the precedence of an operator.
 * Higher number means higher precedence.
 * @param op The operator character.
 * @return An integer representing the precedence level.
 */
static int precedence(char op) {
    switch (op) {
        case '>': return 1;
        case '+': return 2;
        case '*': return 3;
        case '~': return 4;
        default: return 0;
    }
}


// --- Public Function Implementations ---
char *read_line(FILE *stream) {
    size_t capacity = 1024;
    char *buffer = (char *)malloc(capacity);
    if (!buffer) return NULL;

    size_t i = 0;
    int c;

    while ((c = fgetc(stream)) != EOF && c != '\n') {
        buffer[i++] = (char)c;
        if (i >= capacity - 1) {
            capacity *= 2;
            char *new_buffer = (char *)realloc(buffer, capacity);
            if (!new_buffer) {
                free(buffer);
                return NULL;
            }
            buffer = new_buffer;
        }
    }
    buffer[i] = '\0';
    return buffer;
}

Stack *task1_infixToPrefix(const char *infix) {
    int n = strlen(infix);
    if (n == 0) return NULL;

    int capacity = n + 1; // A reasonable capacity

    Stack *op_stack = createStack(capacity);
    Stack *final_stack = createStack(capacity);
    if (!op_stack || !final_stack) {
        freeStack(op_stack);
        freeStack(final_stack);
        return NULL;
    }

    char token_buffer[128];

    for (int i = n - 1; i >= 0; i--) {
        char c = infix[i];

        if (isspace(c)) {
            continue;
        } else if (isOperandStart(c) || isdigit(c)) {
            int k = 0;
            while (i >= 0 && (isOperandStart(infix[i]) || isdigit(infix[i]))) {
                if (k < 127) {
                    token_buffer[k++] = infix[i--];
                } else {
                    i--; // Skip if token is too long
                }
            }
            i++; // Correct for the extra decrement in the while loop
            token_buffer[k] = '\0';
            my_strrev(token_buffer);
            push(final_stack, token_buffer);
        } else if (c == ')') {
            push(op_stack, ")");
        } else if (c == '(') {
            char *op = NULL;
            while ((op = pop(op_stack)) != NULL) {
                if (strcmp(op, ")") == 0) {
                    free(op); // Free the ")"
                    break;
                }
                push(final_stack, op);
                free(op);
            }
        } else if (isOperator(c)) {
            // This block now correctly handles operator precedence.
            while (!isEmpty(op_stack) && strcmp(op_stack->data[op_stack->top], ")") != 0) {
                char top_op_char = op_stack->data[op_stack->top][0];
                int top_prec = precedence(top_op_char);
                int current_prec = precedence(c);

                bool should_pop = false;
                // Pop if top operator has higher precedence.
                if (top_prec > current_prec) {
                    should_pop = true;
                } 
                // If precedence is equal, pop for left-associative operators.
                else if (top_prec == current_prec && c != '>') { // Assume '>' is right-associative
                    should_pop = true;
                }

                if (should_pop) {
                    char* op = pop(op_stack);
                    push(final_stack, op);
                    free(op);
                } else {
                    break; // Stop popping
                }
            }
            // Push the current operator onto the stack.
            char current_op_str[2] = {c, '\0'};
            push(op_stack, current_op_str);
        }
    }

    // Pop remaining operators from op_stack to final_stack
    char *op;
    while ((op = pop(op_stack)) != NULL) {
        push(final_stack, op);
        free(op);
    }
    freeStack(op_stack);

    // The `final_stack` now contains the postfix of the reversed infix.
    // Popping from this stack yields the tokens in the correct prefix order.
    // The previous logic to reverse it into a `return_stack` was incorrect
    // as it produced a stack that popped in reverse-prefix order.

    // --- Print the final prefix expression for verification ---
    // The loop now iterates from top to bottom to show the pop order,
    // which is the human-readable prefix expression.
    printf("Generated Prefix: ");
    for (int i = final_stack->top; i >= 0; i--) {
        printf("%s ", final_stack->data[i]);
    }
    printf("\n\n"); // Add newlines for better formatting in the output

    return final_stack;
}
