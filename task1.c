#include <stdio.h>
#include <stdlib.h>
#include <string.h>

// --- Stack Structure Definition ---
typedef struct {
    char* data;    // Array to hold stack data
    int top;       // Index of the top element
    int capacity;  // Max size of the stack
} Stack;

/**
 * @brief Creates a new stack with a given capacity.
 */
Stack* createStack(int capacity) {
    Stack* stack = (Stack*)malloc(sizeof(Stack));
    if (!stack) {
        fprintf(stderr, "Memory allocation failed for stack struct\n");
        return NULL;
    }
    stack->data = (char*)malloc(sizeof(char) * capacity);
    if (!stack->data) {
        fprintf(stderr, "Memory allocation failed for stack data\n");
        free(stack);
        return NULL;
    }
    stack->capacity = capacity;
    stack->top = -1; // Stack is initially empty
    return stack;
}

/**
 * @brief Checks if the stack is empty.
 */
int isEmpty(Stack* stack) {
    return stack->top == -1;
}

/**
 * @brief Pushes a character onto the stack.
 */
void push(Stack* stack, char item) {
    if (stack->top >= stack->capacity - 1) {
        // This should not happen given our pre-allocation
        fprintf(stderr, "Stack overflow error\n");
        return;
    }
    stack->data[++stack->top] = item;
}

/**
 * @brief Pops a character from the stack.
 */
char pop(Stack* stack) {
    if (isEmpty(stack)) {
        fprintf(stderr, "Stack underflow error\n");
        return '\0'; // Return null char on error
    }
    return stack->data[stack->top--];
}

/**
 * @brief Frees all memory associated with the stack.
 */
void freeStack(Stack* stack) {
    if (stack) {
        free(stack->data);
        free(stack);
    }
}
// --- End of Stack Structure ---


/**
 * @brief Checks if a character is an operand (variable).
 */
int isOperand(char ch) {
    return (ch >= 'a' && ch <= 'z') || (ch >= 'A' && ch <= 'Z');
}

/**
 * @brief Task 1: Converts a fully parenthesized infix propositional
 * logic expression to a prefix expression.
 *
 * @param infix The input infix expression string (e.g., "((a*b)+c)")
 * @return A Stack* containing the prefix expression.
 * The top of the stack is the first character.
 */
Stack* task1_infixToPrefix(const char* infix) {
    int n = strlen(infix);
    if (n == 0) {
        return NULL;
    }

    // --- Internal Stack for the Algorithm ---
    // This stack is temporary and used *only* for the conversion.
    Stack* op_stack = createStack(n);
    if (!op_stack) {
        return NULL;
    }

    // Buffers for intermediate strings
    char* reversed_infix = (char*)malloc(sizeof(char) * (n + 1));
    char* temp_postfix = (char*)malloc(sizeof(char) * (n + 1));
    if (!reversed_infix || !temp_postfix) {
        fprintf(stderr, "Memory allocation failed for buffers\n");
        freeStack(op_stack);
        free(reversed_infix);
        free(temp_postfix);
        return NULL;
    }

    int postfix_index = 0;

    // 1. Reverse infix string and swap parentheses
    for (int i = 0; i < n; i++) {
        char c = infix[n - 1 - i];
        if (c == '(') {
            reversed_infix[i] = ')';
        } else if (c == ')') {
            reversed_infix[i] = '(';
        } else {
            reversed_infix[i] = c;
        }
    }
    reversed_infix[n] = '\0';

    // 2. Perform Shunting-Yard (infix-to-postfix) on the reversed string
    for (int i = 0; i < n; i++) {
        char c = reversed_infix[i];

        if (isOperand(c)) {
            temp_postfix[postfix_index++] = c;
        } else if (c == '(') {
            push(op_stack, c);
        } else if (c == ')') {
            while (!isEmpty(op_stack) && op_stack->data[op_stack->top] != '(') {
                temp_postfix[postfix_index++] = pop(op_stack);
            }
            if (!isEmpty(op_stack)) {
                pop(op_stack); // Discard the '('
            }
        } else if (c == '~' || c == '*' || c == '+' || c == '>') {
            push(op_stack, c);
        }
    }

    // 3. Pop all remaining operators
    while (!isEmpty(op_stack)) {
        temp_postfix[postfix_index++] = pop(op_stack);
    }
    temp_postfix[postfix_index] = '\0';

    // 4. Reverse the temporary postfix string to get the final prefix string
    int m = strlen(temp_postfix);
    char* prefix_result_string = (char*)malloc(sizeof(char) * (m + 1));
    if (!prefix_result_string) {
        fprintf(stderr, "Memory allocation failed for result string\n");
        freeStack(op_stack);
        free(reversed_infix);
        free(temp_postfix);
        return NULL;
    }

    for (int i = 0; i < m; i++) {
        prefix_result_string[i] = temp_postfix[m - 1 - i];
    }
    prefix_result_string[m] = '\0';

    // 5. Clean up temporary allocations
    freeStack(op_stack);
    free(reversed_infix);
    free(temp_postfix);

    // --- Create the Final Output Stack ---
    // This is the stack we will return, as requested.
    Stack* final_stack = createStack(m);
    if (!final_stack) {
        free(prefix_result_string);
        return NULL;
    }
    
    // Push the prefix string onto the stack in reverse order
    // so that popping it reveals the string in correct order.
    // e.g., for ">*ab~c", we push 'c', '~', 'b', 'a', '*', '>'
    for (int i = m - 1; i >= 0; i--) {
        push(final_stack, prefix_result_string[i]);
    }

    // Free the temporary string, as its data is now in the stack
    free(prefix_result_string);

    // 6. Return the final stack
    return final_stack;
}


/**
 * @brief Main function to handle terminal input and run Task 1.
 */
int main() {
    char buffer[1024];

    printf("Enter a fully parenthesized infix propositional logic expression:\n");
    printf("Input: ");

    if (fgets(buffer, sizeof(buffer), stdin) == NULL) {
        fprintf(stderr, "Error reading input\n");
        return 1;
    }
    buffer[strcspn(buffer, "\n")] = 0; // Remove newline

    if (strlen(buffer) == 0) {
        printf("No input provided.\n");
        return 1;
    }

    // Call the function for Task 1
    Stack* prefix_stack = task1_infixToPrefix(buffer);

    if (prefix_stack) {
        printf("\n--- Result ---\n");
        // Print the infix expression as requested
        printf("Infix:   %s\n", buffer);

        // Print the prefix expression by popping from the returned stack
        printf("Prefix:  ");
        while (!isEmpty(prefix_stack)) {
            printf("%c", pop(prefix_stack));
        }
        printf("\n");

        // Free the memory allocated for the stack
        freeStack(prefix_stack);
    } else {
        printf("Failed to convert the expression.\n");
    }

    return 0;
}