#ifndef TASK4_H
#define TASK4_H

// We include Task2.h because our calculateHeight function needs
// to know the definition of the 'TreeNode' structure.
#include "Task2.h"

/**
 * @brief Calculates the height of a binary tree.
 * * The height is defined as the number of edges on the longest path
 * from the root node to a leaf node. An empty tree has a height of -1.
 * * @param root A pointer to the root node of the tree.
 * @return The integer height of the tree.
 */
int calculateHeight(TreeNode *root);

#endif // TASK4_H
