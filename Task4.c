#include <stdio.h>

// Include the header for Task4 to link this implementation
// to its declaration.
#include "Task4.h"
#include "Task2.h"
// Your code is already correct, no changes were needed.
int calculateHeight(TreeNode *root) {
    if (root == NULL) {
        return -1; // Height of an empty tree
    }

    // Recursively find the height of the left and right subtrees
    int leftSubtreeHeight = calculateHeight(root->left);
    int rightSubtreeHeight = calculateHeight(root->right);

    // The height of the tree is 1 + the height of the taller subtree
    if (leftSubtreeHeight > rightSubtreeHeight) {
        return 1 + leftSubtreeHeight;
    } else {
        return 1 + rightSubtreeHeight;
    }
}
