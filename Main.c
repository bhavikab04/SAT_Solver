#include <stdio.h>
#include <stdbool.h> // Needed for bool type and true/false keywords

// Include the headers for all modules
#include "Task2.h"
#include "Task4.h"
#include "Task5.h" // <-- Added this include

int main() {
    // Define the test expression to be used
    const char *test_expression = "!*p>qr";

    printf("--- Main Test Function ---\n");
    printf("Testing with expression: %s\n", test_expression);

    // 1. Build the tree using the function from Task2.c
    TreeNode *root = prefixToTree(test_expression);

    if (!root) {
        printf("Tree construction failed. Exiting.\n");
        return 1; // Indicate error
    }

    // 2. Print the tree structure using the function from Task2.c
    printTreeVertical(root);

    // 3. Calculate the height using the function from Task4.c
    int height = calculateHeight(root);
    printf("\nThe calculated height of the tree is: %d\n", height);

    // --- NEW: Test the evaluation function from Task5.c ---
    printf("\n--- Testing Evaluation ---\n");

    // Define the truth values for the atoms in the expression.
    // You can change these to test different scenarios.
    TruthAssignment assignments[] = {
        {'p', true},
        {'q', false},
        {'r', true}
    };
    int num_assignments = sizeof(assignments) / sizeof(assignments[0]);

    printf("With p=true, q=false, r=true...\n");

    // 4. Call the evaluation function from Task5.c
    bool result = evaluateTree(root, assignments, num_assignments);

    // 5. Print the final boolean result
    printf("The formula evaluates to: %s\n", result ? "true" : "false");

    // Note: A complete program should also include a function to free
    // the memory allocated for the tree nodes.

    return 0; // Indicate success
}

