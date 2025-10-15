#include <stdio.h>
#include <stdlib.h>
#include "Task5.h"

// This is the core recursive evaluation function.
bool evaluateTree(TreeNode *root, const TruthAssignment assignments[], int num_assignments) {
    // Base Case: If the node is null, we can't evaluate it. This is an error condition.
    if (root == NULL) {
        fprintf(stderr, "Error: Attempted to evaluate a NULL node.\n");
        exit(EXIT_FAILURE);
    }

    // Base Case: If the node is an atom (a leaf), find its truth value.
    if (isAtom(root->data)) {
        for (int i = 0; i < num_assignments; i++) {
            if (assignments[i].atom == root->data) {
                return assignments[i].value;
            }
        }
        // If we get here, the atom's value wasn't provided.
        fprintf(stderr, "Error: Truth value for atom '%c' not found.\n", root->data);
        exit(EXIT_FAILURE);
    }

    // Recursive Step: Evaluate the children first.
    bool leftValue = evaluateTree(root->left, assignments, num_assignments);
    bool rightValue; // Only needed for binary operators

    // Apply the operator at the current node to the results from the children.
    switch (root->data) {
        case '!': // NOT
            return !leftValue;
        case '*': // AND
            rightValue = evaluateTree(root->right, assignments, num_assignments);
            return leftValue && rightValue;
        case '+': // OR
            rightValue = evaluateTree(root->right, assignments, num_assignments);
            return leftValue || rightValue;
        case '>': // IMPLIES (P -> Q is equivalent to !P or Q)
            rightValue = evaluateTree(root->right, assignments, num_assignments);
            return !leftValue || rightValue;
        default:
            fprintf(stderr, "Error: Unrecognized operator '%c' in the tree.\n", root->data);
            exit(EXIT_FAILURE);
    }
}
