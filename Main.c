#include <stdio.h>
#include <stdbool.h>
#include <stdlib.h>

// Include all necessary task headers and the new helper header
#include "helper.h"
#include "Task2.h"
#include "Task3.h"
#include "Task4.h"
#include "Task5.h"
#include "Task6.h"
#include "Task7.h"

// Assume the Stack struct and its related functions (createStack, freeStack)
// are defined in one of the included headers or a separate utility file.
typedef struct Stack Stack;

int main()
{
    const char *test_expression = "~*p>qr";

    printf("--- Main Test Function ---\n");
    printf("Testing with expression: %s\n\n", test_expression);

    // --- Task 2: Building the Parse Tree ---
    printf("--- Building Parse Tree from Prefix Expression ---\n");

    // 1. Convert the prefix string to a stack using the function from helper.c
    Stack *prefix_stack = prefixToStack(test_expression);
    TreeNode *root = NULL;

    if (prefix_stack)
    {
        // 2. Build the tree from the stack. prefixToTree will consume the stack.
        root = prefixToTree(prefix_stack);

        // 3. The stack structure itself can now be freed. The tokens it held
        //    were consumed and freed during the tree-building process.
        freeStack(prefix_stack);

        if (root)
        {
            printTreeVertical(root);
        }
        else
        {
            printf("Failed to build the parse tree from the stack.\n");
        }
    }
    else
    {
        printf("Failed to convert the prefix expression to a stack.\n");
    }

    // --- Proceed with other tasks only if the tree was successfully built ---
    if (root == NULL)
    {
        printf("\nAborting further tasks because the parse tree could not be built.\n");
        return 1;
    }

    // --- Task 3: Infix Expression Reconstruction ---
    printf("\n\n--- Task 3: Infix Expression Reconstruction ---\n");
    int bufferLength = getExpLength(root);
    char *infix = (char *)malloc(bufferLength + 1);
    if (infix)
    {
        int pos = 0;
        inOrderTraversal(root, infix, &pos);
        infix[pos] = '\0';
        printf("Reconstructed Infix: %s\n", infix);
        free(infix);
    }
    else
    {
        printf("Memory allocation failed for infix buffer string\n");
    }

    // --- Task 4: Calculate Tree Height ---
    printf("\n\n--- Task 4: Tree Height Calculation ---\n");
    int height = calculateHeight(root);
    printf("The calculated height of the tree is: %d\n", height);

    // --- Task 5: Evaluation and Truth Table ---
    printf("\n\n--- Task 5: Evaluation & Truth Table ---\n");
    TruthAssignment assignments[] = {
        {.literal = "p", .value = true},
        {.literal = "q", .value = false},
        {.literal = "r", .value = true}};
    int num_assignments = sizeof(assignments) / sizeof(assignments[0]);

    printf("With p=T, q=F, r=T, the formula evaluates to: %s\n",
           evaluateTree(root, assignments, num_assignments) ? "True" : "False");

    char **literals_list = NULL;
    int literal_count = collectUniqueLiterals(root, &literals_list);
    if (literal_count > 0)
    {
        printTruthTable(root, literals_list, literal_count, test_expression);
        for (int i = 0; i < literal_count; i++)
        {
            free(literals_list[i]);
        }
        free(literals_list);
    }

    // --- Task 6: Convert to CNF ---
    printf("\n\n--- Task 6: CNF Conversion ---\n");
    TreeNode *cnf_root = CNF_FORMULA(root);
    if (cnf_root)
    {
        printf("Final CNF formula: ");
        print_formula(cnf_root);
        printf("\n");
        freeTree(cnf_root);
    }
    else
    {
        printf("CNF conversion failed.\n");
    }

    // --- Task 7: CNF Validity Check (Independent of the tree) ---
    //printf("\n\n--- Task 7: CNF Validity Check ---\n");
    //const char *cnf_formula_valid = "p|q|~p & ~q|r|q";
    // In Main.c, after Task 6 creates cnf_root...

    if (cnf_root) {
        printf("\n--- Testing CNF Validity (Task 7) ---\n");
        int valid_count = 0;
        int invalid_count = 0;

        // Call the new function with the CNF tree from Task 6
        bool is_tautology = checkCNFValidity(cnf_root, &valid_count, &invalid_count);

        printf("Analysis of the CNF formula:\n");
        printf("- Valid (Tautological) Clauses: %d\n", valid_count);
        printf("- Invalid Clauses: %d\n", invalid_count);
        printf("- Is the entire formula a tautology? %s\n", is_tautology ? "Yes" : "No");

        // Don't forget to free the CNF tree
        freeTree(cnf_root);
    }

    return 0;
}
