#ifndef TASK4_H
#define TASK4_H

#include "Task2.h" // Includes the definition for the TreeNode structure.

/**
 * @file Task4.h
 * @brief Header file for the tree height calculation function.
 *
 * This file declares the function used to find the height of an expression tree.
 */

/**
 * @brief Calculates the height of a given binary tree.
 *
 * The height is the longest path from the top of the tree (the root) to a
 * node at the bottom (a leaf). A tree with only one node has a height of 0.
 * An empty tree is considered to have a height of -1.
 *
 * @param root A pointer to the root node of the tree.
 * @return The height of the tree as an integer.
 */
int calculateHeight(TreeNode *root);

#endif // TASK4_H