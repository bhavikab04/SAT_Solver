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

/**
 * @brief Reverses a string in place.
 * @note This is a helper function, as strrev() is not standard C.
 * @param str The string to be reversed.
 * @return A pointer to the reversed string (same as str).
 */
static char* my_strrev(char* str) {
    if (!str || !*str) return str; // Handle null or empty string
    int i = 0;
    int j = strlen(str) - 1;
    char temp;
    // Swap characters from the ends towards the center
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

/**
 * @brief Creates and initializes a new stack.
 * @param capacity The maximum number of items the stack can hold.
 * @return A pointer to the newly allocated Stack, or NULL on failure.
 */
Stack *createStack(int capacity) {
    // Allocate memory for the stack structure itself
    Stack *stack = (Stack *)malloc(sizeof(Stack));
    if (!stack) return NULL; // Malloc failed

    // Allocate memory for the data array (array of char pointers)
    stack->data = (char **)malloc(sizeof(char *) * capacity);
    if (!stack->data) {
        free(stack); // Clean up partially allocated stack
        return NULL; // Malloc failed
    }
    
    // Initialize stack properties
    stack->capacity = capacity;
    stack->top = -1; // Stack is initially empty
    return stack;
}

/**
 * @brief Checks if the stack is empty.
 * @param stack Pointer to the stack.
 * @return 1 (true) if empty, 0 (false) otherwise.
 */
int isEmpty(Stack *stack) { return stack->top == -1; }

/**
 * @brief Pushes a string item onto the stack.
 * @note This function creates a copy of the item string using strdup.
 * @param stack Pointer to the stack.
 * @param item The string to be pushed.
 */
void push(Stack *stack, const char *item) {
    // Check for stack overflow
    if (stack->top >= stack->capacity - 1) {
        fprintf(stderr, "Stack overflow error\n");
        return;
    }
    
    // Increment top and add a copy of the item
    stack->data[++stack->top] = strdup(item); 
    
    // Check if strdup failed
    if (stack->data[stack->top] == NULL) {
        fprintf(stderr, "Memory allocation failed for push\n");
        stack->top--; // Revert the top increment
    }
}

/**
 * @brief Pops a string item from the stack.
 * @note The caller is responsible for freeing the returned string.
 * @param stack Pointer to the stack.
 * @return The string from the top of the stack, or NULL if empty.
 */
char *pop(Stack *stack) {
    // Check for stack underflow
    if (isEmpty(stack)) {
        return NULL;
    }
    // Return the top item and decrement the top index
    return stack->data[stack->top--];
}

/**
 * @brief Frees all memory associated with the stack.
 * @param stack Pointer to the stack to be freed.
 */
void freeStack(Stack *stack) {
    if (stack) {
        // Pop and free every string item remaining in the stack
        while (!isEmpty(stack)) {
            free(pop(stack)); // pop() returns the pointer, free() deallocates it
        }
        // Free the data array itself
        free(stack->data);
        // Free the stack structure
        free(stack);
    }
}
// --- End of Stack Implementation ---


// --- Private Helper Functions ---

/**
 * @brief Checks if a character is a supported operator.
 * @param ch The character to check.
 * @return 1 (true) if it's an operator, 0 (false) otherwise.
 */
static int isOperator(char ch) {
    return ch == '~' || ch == '*' || ch == '+' || ch == '>';
}

/**
 * @brief Checks if a character can be the start of an operand (a letter).
 * @param ch The character to check.
 * @return 1 (true) if it's a letter, 0 (false) otherwise.
 */
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
        case '>': return 1; // Implication
        case '+': return 2; // OR
        case '*': return 3; // AND
        case '~': return 4; // NOT (highest precedence)
        default: return 0; // Not an operator
    }
}


// --- Public Function Implementations ---

/**
 * @brief Reads a line of arbitrary length from a stream.
 * @param stream The file stream (e.g., stdin).
 * @return A dynamically allocated string containing the line, or NULL on failure.
 * The caller must free this string.
 */
char *read_line(FILE *stream) {
    size_t capacity = 1024; // Initial buffer size
    char *buffer = (char *)malloc(capacity);
    if (!buffer) return NULL; // Malloc failed

    size_t i = 0; // Current index in buffer
    int c; // Character read (as int to check for EOF)

    // Read characters one by one until newline or EOF
    while ((c = fgetc(stream)) != EOF && c != '\n') {
        buffer[i++] = (char)c;
        
        // Check if buffer needs to be resized
        if (i >= capacity - 1) {
            capacity *= 2; // Double the capacity
            char *new_buffer = (char *)realloc(buffer, capacity);
            if (!new_buffer) {
                free(buffer); // Free old buffer on realloc failure
                return NULL;
            }
            buffer = new_buffer; // Point to the new, larger buffer
        }
    }
    buffer[i] = '\0'; // Null-terminate the string
    return buffer;
}

/**
 * @brief Converts an infix expression string to a prefix expression stack.
 * * This function implements the Shunting-yard algorithm modified for
 * infix-to-prefix, which involves scanning the infix string from
 * RIGHT to LEFT.
 *
 * @param infix The null-terminated infix expression string.
 * @return A pointer to a Stack containing the prefix expression tokens.
 * The tokens are ordered such that popping from the stack gives
 * the reverse prefix order, but iterating the stack->data array
 * from top-to-bottom gives the correct prefix order.
 * Returns NULL on failure or empty input.
 */
Stack *task1_infixToPrefix(const char *infix) {
    int n = strlen(infix);
    if (n == 0) return NULL; // Handle empty input string

    int capacity = n + 1; // A reasonable initial capacity for the stacks

    // op_stack holds operators and parentheses
    Stack *op_stack = createStack(capacity);
    // final_stack holds the output (postfix of the reversed string)
    Stack *final_stack = createStack(capacity);
    
    // Check if stack creation failed
    if (!op_stack || !final_stack) {
        freeStack(op_stack); // freeStack handles NULL safely
        freeStack(final_stack);
        return NULL;
    }

    // Temporary buffer to build multi-character tokens (operands)
    char token_buffer[128]; 

    // --- Main algorithm loop: Scan infix string from RIGHT to LEFT ---
    for (int i = n - 1; i >= 0; i--) {
        char c = infix[i];

        if (isspace(c)) {
            continue; // Ignore whitespace
        
        // --- Case 1: Operand ---
        } else if (isOperandStart(c) || isdigit(c)) {
            int k = 0; // Index for token_buffer
            
            // Read the full operand (backwards)
            while (i >= 0 && (isOperandStart(infix[i]) || isdigit(infix[i]))) {
                if (k < 127) { // Avoid buffer overflow
                    token_buffer[k++] = infix[i--];
                } else {
                    i--; // Skip if token is too long
                }
            }
            i++; // Correct for the extra decrement in the while loop
            token_buffer[k] = '\0'; // Null-terminate the token
            
            // Since we read it backwards, reverse it back to normal
            my_strrev(token_buffer); 
            
            // Push the operand onto the final output stack
            push(final_stack, token_buffer);
        
        // --- Case 2: Closing Parenthesis (since scanning right-to-left) ---
        } else if (c == ')') {
            push(op_stack, ")"); // Push onto operator stack
        
        // --- Case 3: Opening Parenthesis (since scanning right-to-left) ---
        } else if (c == '(') {
            char *op = NULL;
            // Pop operators from op_stack to final_stack until ')' is found
            while ((op = pop(op_stack)) != NULL) {
                if (strcmp(op, ")") == 0) {
                    free(op); // Discard the ")"
                    break;    // Stop popping
                }
                push(final_stack, op); // Push operator to final stack
                free(op); // Free the popped string
            }
        
        // --- Case 4: Operator ---
        } else if (isOperator(c)) {
            // This block handles operator precedence.
            // Pop operators from op_stack if they have higher or equal precedence
            // than the current operator 'c'.
            while (!isEmpty(op_stack) && strcmp(op_stack->data[op_stack->top], ")") != 0) {
                char top_op_char = op_stack->data[op_stack->top][0]; // Peek at top operator
                int top_prec = precedence(top_op_char);
                int current_prec = precedence(c);

                bool should_pop = false;
                
                // Pop if top operator has *higher* precedence.
                if (top_prec > current_prec) {
                    should_pop = true;
                } 
                // If precedence is equal, pop for left-associative operators.
                // Since we scan R-to-L, we pop on equal precedence for L-assoc ops
                // and push for R-assoc ops. '>' is right-associative.
                else if (top_prec == current_prec && c != '>') { 
                    should_pop = true;
                }

                if (should_pop) {
                    char* op = pop(op_stack);
                    push(final_stack, op);
                    free(op);
                } else {
                    break; // Stop popping (current op has higher or R-assoc equal prec)
                }
            }
            
            // Push the current operator onto the op_stack.
            char current_op_str[2] = {c, '\0'}; // Convert char to string
            push(op_stack, current_op_str);
        }
    } // --- End of main scanning loop ---

    // --- Final Step: Pop remaining operators ---
    // Pop any remaining operators from op_stack to final_stack
    char *op;
    while ((op = pop(op_stack)) != NULL) {
        push(final_stack, op);
        free(op);
    }
    freeStack(op_stack); // op_stack is no longer needed

    // The `final_stack` now contains the prefix expression.
    // The items are in the correct order in the data array
    // from top (index final_stack->top) down to bottom (index 0).

    // --- Print the final prefix expression for verification ---
    // This loop iterates from the top of the stack data array downwards,
    // which prints the tokens in the correct human-readable prefix order.
    printf("Generated Prefix: ");
    for (int i = final_stack->top; i >= 0; i--) {
        printf("%s ", final_stack->data[i]);
    }
    printf("\n\n"); // Add newlines for better formatting in the output

    // Return the final stack containing the prefix expression.
    return final_stack;
}