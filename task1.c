#include <stdio.h>
#include <stdlib.h>
#include <string.h>
#include <ctype.h>
//We can't use the previous reverse-string algorithm for this for large CNF files as it'll be too memory inefficient
//More dynamics, pushes the tokens into stacks directly
// --- Stack Structure (now stores strings) ---
typedef struct {
    char** data;   // Array of strings (char*)
    int top;       // Index of the top element
    int capacity;  // Max size of the stack
} Stack;

/**
 * @brief Creates a new stack with a given capacity.
 */
Stack* createStack(int capacity) {
    Stack* stack = (Stack*)malloc(sizeof(Stack));
    if (!stack) return NULL;
    
    // Allocate space for an array of (char*) pointers
    stack->data = (char**)malloc(sizeof(char*) * capacity);
    if (!stack->data) {
        free(stack);
        return NULL;
    }
    stack->capacity = capacity;
    stack->top = -1;
    return stack;
}

/**
 * @brief Checks if the stack is empty.
 */
int isEmpty(Stack* stack) {
    return stack->top == -1;
}

/**
 * @brief Pushes a string onto the stack (makes a copy).
 */
void push(Stack* stack, const char* item) {
    if (stack->top >= stack->capacity - 1) {
        // In a real-world scenario, we'd realloc here.
        fprintf(stderr, "Stack overflow error\n");
        return;
    }
    // strdup allocates memory for a copy of the string
    stack->data[++stack->top] = strdup(item);
    if (stack->data[stack->top] == NULL) {
        fprintf(stderr, "Memory allocation failed for push\n");
        stack->top--; // Roll back
    }
}

/**
 * @brief Pops a string from the stack (transfers ownership).
 */
char* pop(Stack* stack) {
    if (isEmpty(stack)) {
        return NULL;
    }
    // Caller is now responsible for freeing this string
    return stack->data[stack->top--];
}

/**
 * @brief Frees all memory associated with the stack, including all strings.
 */
void freeStack(Stack* stack) {
    if (stack) {
        // Free all the strings stored in the stack
        while (!isEmpty(stack)) {
            free(pop(stack));
        }
        free(stack->data); // Free the array of pointers
        free(stack);       // Free the stack struct
    }
}
// --- End of Stack Structure ---


/**
 * @brief Checks if a character is one of the operators.
 */
int isOperator(char ch) {
    return ch == '~' || ch == '*' || ch == '+' || ch == '>';
}

/**
 * @brief Checks if a character starts an operand (e.g., 'x' in 'x123').
 */
int isOperandStart(char ch) {
    // We assume variables are like 'x1', 'x24', or single letters 'a', 'b'
    return (ch >= 'a' && ch <= 'z') || (ch >= 'A' && ch <= 'Z');
}

/**
 * @brief Dynamically reads a single line from a file stream.
 * @return A new, heap-allocated string. Caller must free().
 */
char* read_line(FILE* stream) {
    size_t capacity = 1024;
    char* buffer = (char*)malloc(capacity);
    if (!buffer) return NULL;

    size_t i = 0;
    int c;

    while ((c = fgetc(stream)) != EOF && c != '\n') {
        buffer[i++] = (char)c;
        // Grow buffer if full
        if (i >= capacity - 1) {
            capacity *= 2;
            char* new_buffer = (char*)realloc(buffer, capacity);
            if (!new_buffer) {
                free(buffer);
                return NULL;
            }
            buffer = new_buffer;
        }
    }
    buffer[i] = '\0'; // Null-terminate the string
    return buffer;
}


/**
 * @brief Task 1: Converts a fully parenthesized infix expression
 * to a prefix expression using a right-to-left scan.
 *
 * @param infix The input infix expression string.
 * @return A Stack* containing the prefix expression tokens.
 * The top of the stack is the first token.
 */
Stack* task1_infixToPrefix(const char* infix) {
    int n = strlen(infix);
    if (n == 0) return NULL;

    Stack* op_stack = createStack(n);    // Stack for operators
    Stack* final_stack = createStack(n); // Stack for final prefix expression
    if (!op_stack || !final_stack) {
        freeStack(op_stack);
        freeStack(final_stack);
        return NULL;
    }

    char token_buffer[128]; // Buffer for a single token (e.g., "x12345")

    // --- Right-to-Left Scan ---
    for (int i = n - 1; i >= 0; i--) {
        char c = infix[i];

        if (isspace(c)) {
            continue; // Skip whitespace
        }
        // --- 1. Handle Operands (e.g., "x123", "a") ---
        // We read them backwards, so we find '3', '2', '1', 'x'
        else if (isdigit(c) || isOperandStart(c)) {
            int k = 0;
            // Keep reading while it's part of the variable name
            while (i >= 0 && (isOperandStart(infix[i]) || isdigit(infix[i]))) {
                if (k < 127) {
                    token_buffer[k++] = infix[i--];
                } else {
                    i--; // Skip if token is too long
                }
            }
            i++; // Loop will decrement again, so we adjust
            token_buffer[k] = '\0';

            // Now, re-reverse the token (we read "321x", we want "x123")
            strrev(token_buffer); // Use _strrev on Windows
            
            // Push the completed operand onto the final stack
            push(final_stack, token_buffer);
        }
        // --- 2. Handle ')' ---
        else if (c == ')') {
            push(op_stack, ")"); // Push ")" (as a string)
        }
        // --- 3. Handle '(' ---
        else if (c == '(') {
            char* op;
            while (!isEmpty(op_stack) && strcmp((op = pop(op_stack)), ")") != 0) {
                push(final_stack, op); // Pop ops to final stack
                free(op); // We're done with this token
            }
            free(op); // Free the ")" token
        }
        // --- 4. Handle Operators ---
        else if (isOperator(c)) {
            // Because it's fully parenthesized, we don't need precedence.
            // But if we did, we'd pop operators here.
            token_buffer[0] = c;
            token_buffer[1] = '\0';
            push(op_stack, token_buffer);
        }
    }

    // --- 5. Pop remaining operators ---
    char* op;
    while ((op = pop(op_stack)) != NULL) {
        push(final_stack, op);
        free(op);
    }

    freeStack(op_stack);

    // --- 6. Reverse the final stack ---
    // The `final_stack` has the prefix expression, but reversed.
    // e.g., for ">*ab~c", it has 'c', '~', 'b', 'a', '*', '>' (top)
    // We must return a stack that pops in the *correct* order.
    Stack* return_stack = createStack(n);
    while ((op = pop(final_stack)) != NULL) {
        push(return_stack, op);
        free(op);
    }

    freeStack(final_stack);
    return return_stack;
}


int main() {
    printf("Enter a fully parenthesized infix propositional logic expression:\n");
    printf("Input: ");

    char* buffer = read_line(stdin); // Dynamically read line
    if (buffer == NULL) {
        fprintf(stderr, "Error reading input\n");
        return 1;
    }

    if (strlen(buffer) == 0) {
        printf("No input provided.\n");
        free(buffer);
        return 1;
    }

    Stack* prefix_stack = task1_infixToPrefix(buffer);

    if (prefix_stack) {
        printf("\n--- Result ---\n");
        printf("Infix:   %s\n", buffer);

        // Print the prefix expression by popping from the returned stack
        printf("Prefix:  ");
        char* token;
        while ((token = pop(prefix_stack)) != NULL) {
            printf("%s ", token); // Print token with a space
            free(token); // Free the token
        }
        printf("\n");

        freeStack(prefix_stack);
    } else {
        printf("Failed to convert the expression.\n");
    }

    free(buffer); // Free the input line buffer
    return 0;
}