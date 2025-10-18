#include <stdio.h>
#include <stdbool.h>
#include <stdlib.h>
#include "Task1.c"

#include "Task2.h"
#include "Task4.h"
#include "Task3.h"
#include "Task5.h"
#include "Task6.h"
#include "Task7.h"

int main()
{
    const char *test_expression = "~*p>qr";

    printf("--- Main Test Function ---\n");
    printf("Testing with expression: %s\n", test_expression);

    // Task 2:

    // ... input reading logic ...

    Stack *prefix_stack = task1_infixToPrefix(buffer);

    if (prefix_stack)
    {
        printf("\n--- Task 1 Result ---\n");
        // NOTE: The stack is printed by popping, which consumes it.
        // We must *replicate* the stack if we want to print AND use it for Task 2.
        // For simplicity, let's assume Task 1's main prints it first.

        // ... (Code to print prefix and free/re-create the stack, OR a simpler approach) ...

        printf("\n--- Task 2 Execution ---\n");
        // ** The prefixToTree function will consume (pop) the stack.**
        TreeNode *root = prefixToTree(prefix_stack);

        if (root)
        {
            printTreeVertical(root);
            // TODO: Add function to free the tree memory (not shown here)
        }
        else
        {
            printf("Failed to build the parse tree.\n");
        }

        // Since prefixToTree consumed the stack, freeStack only frees the remaining structure/memory.
        // If the stack was completely consumed (which it should be), this is safe.
        freeStack(prefix_stack);
    }
    else
    {
        printf("Failed to convert the expression.\n");
    }

    return 0;

    // Task 3:
    printf("\n--- Infix Expression Reconstruction (In-order Traversal) ---\n");

    int bufferLength = getExpLength(root);
    char *infix = (char *)malloc(bufferLength + 1);

    if (!infix)
    {
        printf("Memory allocation failed for infix buffer string\n");
        freeTree(root);
        return 1;
    }
    int pos = 0;
    inOrderTraversal(root, infix, &pos);
    infix[pos] = '\0';

    free(infix);
    // TreeNode *root_new = prefixToTree(test_expression);
    // Task 4:
    int height = calculateHeight(root);
    printf("\nThe calculated height of the tree is: %d\n", height);

    printf("\n--- Testing Evaluation ---\n");
    // Task 5:
    TruthAssignment assignments[] = {
        {'p', true},
        {'q', false},
        {'r', true}};
    int num_assignments = sizeof(assignments) / sizeof(assignments[0]);

    printf("With p=true, q=false, r=true...\n");
    if (root == NULL)
    {
        printf("NULL node");
    }
    bool result = evaluateTree(root, assignments, num_assignments);
    printf("The formula evaluates to: %s\n", result ? "true" : "false");

    // Task 6: Convert the formula to CNF form
    //-------------------------------------------------------------
    printf("\n--- Testing CNF Conversion (Task 6) ---\n");

    // Pass the root of the existing parse tree to the CNF conversion function.
    // The CNF_FORMULA() function will create and return a new tree
    // that represents the formula in CNF (Conjunctive Normal Form).
    TreeNode *cnf_root = CNF_FORMULA(root);

    if (cnf_root)
    {
        printf("\nCNF Conversion Complete.\n");
        printf("Here’s the final CNF formula (conversion steps were shown above):\n");

        // Print the converted CNF formula neatly
        print_formula(cnf_root);
        printf("\n");

        // Free up the memory used by the new CNF tree to avoid memory leaks
        freeTree(cnf_root);
    }
    else
    {
        printf("CNF conversion failed. Something went wrong during processing.\n");
    }
    //-------------------------------------------------------------

    // Task 7:
    printf("\n--- Testing CNF Validity (Task 7) ---\n");

    const char *cnf_formula_valid = "p|q|~p & ~q|r|q";
    const char *cnf_formula_invalid = "a | b & c | ~b";

    printf("Checking formula: \"%s\"\n", cnf_formula_valid);
    bool cnf_result_1 = isValidCNF(cnf_formula_valid);
    printf("Is the formula valid? %s\n\n", cnf_result_1 ? "Yes" : "No");

    printf("Checking formula: \"%s\"\n", cnf_formula_invalid);
    bool cnf_result_2 = isValidCNF(cnf_formula_invalid);
    printf("Is the formula valid? %s\n", cnf_result_2 ? "Yes" : "No");

    freeTree(root);

    return 0;
}
