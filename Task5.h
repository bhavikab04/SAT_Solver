#ifndef TASK5_H
#define TASK5_H

#include <stdbool.h>
#include "Task2.h" // We need the definition of TreeNode

/**
 * @brief A structure to map a propositional atom (like 'p') to its truth value.
 */
typedef struct TruthAssignment {
    char atom;
    bool value;
} TruthAssignment;

/**
 * @brief Evaluates the truth value of a parse tree given truth values for its atoms.
 *
 * This function traverses the tree in a bottom-up (post-order) fashion.
 * It recursively evaluates the children of a node before applying the
 * operator at the node itself.
 *
 * @param root A pointer to the root node of the parse tree.
 * @param assignments An array of TruthAssignment structs that provide the boolean
 * values for each atom in the formula.
 * @param num_assignments The number of elements in the assignments array.
 * @return The final boolean truth value of the entire formula.
 */
bool evaluateTree(TreeNode *root, const TruthAssignment assignments[], int num_assignments);

#endif // TASK5_H
