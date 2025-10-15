#include <stdio.h>
#include <stdbool.h> 
#include <stdlib.h>
#include "Task2.h"
#include "Task4.h"
#include "Task3.h"
#include "Task5.h" 

#include "Task7.h" 
int main() {
    const char *test_expression = "!*p>qr";

    printf("--- Main Test Function ---\n");
    printf("Testing with expression: %s\n", test_expression);

    //Task 2:

    TreeNode *root = prefixToTree(test_expression);

    if (!root) {
        printf("Tree construction failed. Exiting.\n");
        return 1; 
    }

    printTreeVertical(root);

    //Task 3:
    printf("\n--- Infix Expression Reconstruction (In-order Traversal) ---\n");

    int bufferLength = getExpLength(root);
    char* infix = (char*)malloc(bufferLength+1);

    if (!infix){
        printf("Memory allocation failed for infix buffer string\n");
        freeTree(root);
        return 1;
    }
    int pos = 0;
    inOrderTraversal(root, infix, &pos); 
    infix[pos] = '\0'; 

    free(infix);

    //Task 4:
    int height = calculateHeight(root);
    printf("\nThe calculated height of the tree is: %d\n", height);

    printf("\n--- Testing Evaluation ---\n");
    //Task 5:
    TruthAssignment assignments[] = {
        {'p', true},
        {'q', false},
        {'r', true}
    };
    int num_assignments = sizeof(assignments) / sizeof(assignments[0]);

    printf("With p=true, q=false, r=true...\n");

    bool result = evaluateTree(root, assignments, num_assignments);
    printf("The formula evaluates to: %s\n", result ? "true" : "false");

    //Task 7:
    printf("\n--- Testing CNF Validity (Task 7) ---\n");
    
    const char* cnf_formula_valid = "p|q|~p & ~q|r|q";
    const char* cnf_formula_invalid = "a | b & c | ~b";

    printf("Checking formula: \"%s\"\n", cnf_formula_valid);
    bool cnf_result_1 = isValidCNF(cnf_formula_valid);
    printf("Is the formula valid? %s\n\n", cnf_result_1 ? "Yes" : "No");

    printf("Checking formula: \"%s\"\n", cnf_formula_invalid);
    bool cnf_result_2 = isValidCNF(cnf_formula_invalid);
    printf("Is the formula valid? %s\n", cnf_result_2 ? "Yes" : "No");
    
    freeTree(root);

    return 0; 
}

