#include <stdio.h>
#include <stdlib.h>
#include <string.h>

/**
 * @brief Checks if a character is an operand (variable).
 * We assume variables are single letters.
 */
int isOperand(char ch) {
    return (ch >= 'a' && ch <= 'z') || (ch >= 'A' && ch <= 'Z');
}

/**
 * @brief Task 1: Converts a fully parenthesized infix propositional
 * logic expression to a prefix expression.
 *
 * This function implements the reverse-postfix-reverse algorithm
 * and uses an internal array-based stack for efficiency and to
 * adhere to the "one function" constraint.
 *
 * @param infix The input infix expression string (e.g., "((a*b)+c)")
 * @return A new string (char*) containing the prefix expression.
 * The caller is responsible for freeing this memory.
 */
char* task1_infixToPrefix(const char* infix) {
    int n = strlen(infix);
    if (n == 0) {
        return NULL;
    }

    // --- Stack Structure (Internal Implementation) ---
    // We create the stack with the maximum possible size (n)
    // to avoid dynamic resizing or complex checks.
    char* stack = (char*)malloc(sizeof(char) * n);
    if (!stack) {
        fprintf(stderr, "Memory allocation failed for stack\n");
        return NULL;
    }
    int top = -1; // Stack pointer
    // --- End of Stack Structure ---

    // Buffers for intermediate strings
    char* reversed_infix = (char*)malloc(sizeof(char) * (n + 1));
    char* temp_postfix = (char*)malloc(sizeof(char) * (n + 1));
    if (!reversed_infix || !temp_postfix) {
        fprintf(stderr, "Memory allocation failed for buffers\n");
        free(stack);
        free(reversed_infix); // free(NULL) is safe
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

        // If operand, add to output
        if (isOperand(c)) {
            temp_postfix[postfix_index++] = c;
        }
        // If '(', push to stack
        else if (c == '(') {
            stack[++top] = c; // Push
        }
        // If ')', pop from stack to output until '(' is found
        else if (c == ')') {
            while (top != -1 && stack[top] != '(') {
                temp_postfix[postfix_index++] = stack[top--]; // Pop
            }
            if (top != -1) {
                top--; // Discard the '('
            }
        }
        // If operator (~, *, +, >)
        else if (c == '~' || c == '*' || c == '+' || c == '>') {
            // *** CRITICAL SIMPLIFICATION ***
            // Because the input is "fully parenthesized", we don't
            // need to check for precedence. The parentheses
            // already dictate the order. We just push the operator.
            stack[++top] = c; // Push
        }
    }

    // 3. Pop all remaining operators from the stack to the output
    while (top != -1) {
        temp_postfix[postfix_index++] = stack[top--]; // Pop
    }
    temp_postfix[postfix_index] = '\0'; // Null-terminate the postfix string

    // 4. Reverse the temporary postfix string to get the final prefix string
    int m = strlen(temp_postfix);
    char* prefix_result = (char*)malloc(sizeof(char) * (m + 1));
    if (!prefix_result) {
        fprintf(stderr, "Memory allocation failed for result\n");
        // Free all other allocations
        free(stack);
        free(reversed_infix);
        free(temp_postfix);
        return NULL;
    }

    for (int i = 0; i < m; i++) {
        prefix_result[i] = temp_postfix[m - 1 - i];
    }
    prefix_result[m] = '\0'; // Null-terminate the final prefix string

    // 5. Clean up all allocated memory
    free(stack);
    free(reversed_infix);
    free(temp_postfix);

    // Return the final result
    return prefix_result;
}


/**
 * @brief Main function to handle terminal input and run Task 1.
 */
int main() {
    char buffer[1024];

    printf("Enter a fully parenthesized infix propositional logic expression:\n");
    printf("(Use ~ for NOT, + for OR, * for AND, > for IMPLIES)\n");
    printf("Example: ((a*b)>(~c))\n");
    printf("Input: ");

    // Read input from the terminal
    if (fgets(buffer, sizeof(buffer), stdin) == NULL) {
        fprintf(stderr, "Error reading input\n");
        return 1;
    }

    // Remove trailing newline character from fgets
    buffer[strcspn(buffer, "\n")] = 0;

    // Check for empty input
    if (strlen(buffer) == 0) {
        printf("No input provided.\n");
        return 1;
    }

    // Call the function for Task 1
    char* prefix_expression = task1_infixToPrefix(buffer);

    if (prefix_expression) {
        printf("\n--- Result ---\n");
        printf("Infix:   %s\n", buffer);
        printf("Prefix:  %s\n", prefix_expression);

        // Free the memory allocated by the function
        free(prefix_expression);
    } else {
        printf("Failed to convert the expression.\n");
    }

    return 0;
}