#include <stdio.h>
#include "Task4.h"
#include "Task2.h"

/**
 * @file Task4.c
 * @brief Implements height of Tree calculation
 *
 * This file provides the actual code that performs the height calculation
 * using a recursive method.
 */

/**
 * @brief Determines the height of a tree by checking its subtrees.
 *
 * This function works by recursively finding the height of the left and right
 * branches from the current node. It then takes the larger of those two heights
 * and adds one to it to account for the current level. This process repeats
 * until the bottom of the tree is reached.
 *
 * @param root A pointer to the root node of the tree.
 * @return The integer height of the tree. It returns -1 if the tree is empty.
 */
int calculateHeight(TreeNode *root) {
    // An empty tree (NULL) is defined to have a height of -1.
    if (root == NULL) {
        return -1;
    }

    // Find the height of the left and right branches from this node.
    int leftSubtreeHeight = calculateHeight(root->left);
    int rightSubtreeHeight = calculateHeight(root->right);

    // The height at this point is 1 plus the height of the taller branch.
    if (leftSubtreeHeight > rightSubtreeHeight) {
        return 1 + leftSubtreeHeight;
    } else {
        return 1 + rightSubtreeHeight;
    }
}