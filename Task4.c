#include <stdio.h>
#include "Task4.h"
#include "Task2.h"

/**
 * @brief Provides the function to calculate the height of an expression tree.
 *
 * This file contains the implementation for determining the height of a binary
 * tree, which is defined as the number of edges on the longest path from the
 * root node to a leaf node.
 */

/**
 * @brief Calculates the height of a binary tree using recursion.
 * @param root A pointer to the root node of the tree (or sub-tree).
 * @return The height of the tree. The height of an empty tree (NULL) is -1,
 * and the height of a tree with a single node is 0.
 *
 * This function determines the tree's height by breaking the problem down.
 * It recursively finds the height of the left and right sub-trees. The height
 * of the current node's tree is then calculated as 1 plus the height of whichever
 * sub-tree is taller. This process continues until it reaches the leaves.
 */
int calculateHeight(TreeNode *root) {
    // empty tree with no nodes, height is considered -1
    if (root == NULL) {
        return -1;
    }
    
    // Recursively find the height of the left and right branches.
    int leftSubtreeHeight = calculateHeight(root->left);
    int rightSubtreeHeight = calculateHeight(root->right);

    // The height of the tree rooted at this node  for the current level is 1
    // plus the height of the deepest sub-tree
    if (leftSubtreeHeight > rightSubtreeHeight) {
        return 1 + leftSubtreeHeight;
    } else {
        return 1 + rightSubtreeHeight;
    }
}
