#include <stdio.h>
#include <stdlib.h>
#include <stdbool.h>
#include "Task5.h"
#include "Task2.h" 

/**
 * @brief Evaluates the truth value of a propositional logic formula
 * based on a given
 * set of assignments for the atoms.
 */

/**
 * @brief Recursively evaluates the truth value of an expression tree.
 * @param root A pointer to the root node of the tree or sub-tree to be evaluated.
 * @param assignments An array of `TruthAssignment` structs that maps each atom (like 'p', 'q') to a boolean value.
 * @param num_assignments The number of elements in the `assignments` array.
 * @return Returns `true` or `false` representing the final truth value of the expression.
 *
 * This function walks down the tree until it
 * hits the leaves, where it looks up their truth values. It then walks
 * back up, applying the logical operators at each node to the results from its children.
 * If it ever encounters a NULL node or an unknown operator, it treats it as an
 * error and halts the program.
 */
bool evaluateTree(TreeNode *root, const TruthAssignment assignments[], int num_assignments) {
    if (root == NULL) {
        fprintf(stderr, "Error: Attempted to evaluate a NULL node.\n");
        exit(EXIT_FAILURE);
    }

    if (isAtom(root->data)) {
        for (int i = 0; i < num_assignments; i++) {
            if (assignments[i].atom == root->data) {
                return assignments[i].value;
            }
        }
        fprintf(stderr, "Error: Truth value for atom '%c' not found.\n", root->data);
        exit(EXIT_FAILURE);
    }

 
    switch (root->data) {
        case '~': { // NOT
            // For a unary operator like NOT, we only need to evaluate one child.
            // Based on the convention used in class, we look at the right node.
            bool childValue = evaluateTree(root->right, assignments, num_assignments);
            return !childValue;
        }
        case '*': { // AND
            // For binary operators, we evaluate both the left and right children
            // and then combine their results.
            bool leftValue = evaluateTree(root->left, assignments, num_assignments);
            bool rightValue = evaluateTree(root->right, assignments, num_assignments);
            return leftValue && rightValue;
        }
        case '+': { // OR
            bool leftValue = evaluateTree(root->left, assignments, num_assignments);
            bool rightValue = evaluateTree(root->right, assignments, num_assignments);
            return leftValue || rightValue;
        }
        case '>': { // IMPLIES (material implication)
            // So P -> Q as the equivalent expression: not P or Q.
            bool leftValue = evaluateTree(root->left, assignments, num_assignments);
            bool rightValue = evaluateTree(root->right, assignments, num_assignments);
            return !leftValue || rightValue;
        }
        default:
            // If the character isn't an atom or a recognized operator, the tree
            // is invalid.
            fprintf(stderr, "Error: Unrecognized operator '%c' in the tree.\n", root->data);
            exit(EXIT_FAILURE);
    }
}

