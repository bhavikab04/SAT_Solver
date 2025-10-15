#include <stdio.h>
#include <stdlib.h>
#include <stdbool.h>
#include "Task5.h"
#include "Task2.h" 


// recursive evaluation function.
bool evaluateTree(TreeNode *root, const TruthAssignment assignments[], int num_assignments) {
    // Base Case: If the node is null, we can't evaluate it. This is an error condition.
    if (root == NULL) {
        fprintf(stderr, "Error: Attempted to evaluate a NULL node.\n");
        exit(EXIT_FAILURE);
    }

    // Base Case: If the node is a leaf, find its truth value.
    if (isAtom(root->data)) {
        for (int i = 0; i < num_assignments; i++) {
            if (assignments[i].atom == root->data) {
                return assignments[i].value;
            }
        }
        // If we get here, the leaf's value wasn't provided.
        fprintf(stderr, "Error: Truth value for atom '%c' not found.\n", root->data);
        exit(EXIT_FAILURE);
    }

    // Recursive Step: Evaluate children based on the operator.
    switch (root->data) {
        case '~': { // NOT
            // A unary operator should have its child in a consistent location.
            // We assume it's in root->right as requested.
            // Your prefixToTree function MUST also place the child in root->right.
            bool childValue = evaluateTree(root->right, assignments, num_assignments);
            return !childValue;
        }
        case '*': { // AND
            bool leftValue = evaluateTree(root->left, assignments, num_assignments);
            bool rightValue = evaluateTree(root->right, assignments, num_assignments);
            return leftValue && rightValue;
        }
        case '+': { // OR
            bool leftValue = evaluateTree(root->left, assignments, num_assignments);
            bool rightValue = evaluateTree(root->right, assignments, num_assignments);
            return leftValue || rightValue;
        }
        case '>': { // IMPLIES (P -> Q is equivalent to !P or Q)
            bool leftValue = evaluateTree(root->left, assignments, num_assignments);
            bool rightValue = evaluateTree(root->right, assignments, num_assignments);
            return !leftValue || rightValue;
        }
        default:
            fprintf(stderr, "Error: Unrecognized operator '%c' in the tree.\n", root->data);
            exit(EXIT_FAILURE);
    }
}

