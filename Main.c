#include <stdio.h>

// Include the headers for both modules to access their functions
#include "Task2.h"
#include "Task4.h"

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
    // This visually confirms the tree was built correctly.
    printTreeVertical(root);

    // 3. Calculate the height using the function from Task4.c
    int height = calculateHeight(root);

    // 4. Print the final height
    printf("\nThe calculated height of the tree is: %d\n", height);

    // Note: A complete program should also include a function to free
    // the memory allocated for the tree nodes.

    return 0; // Indicate success
}

