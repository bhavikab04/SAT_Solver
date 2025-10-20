/**
 * @file parse_tree_to_infix.h
 *
 * @brief Implentation of functions to construct infix expression from parse tree by in-order traversal.
 */

#ifndef TASK3_H
#define TASK3_H

// Expression length calculator function:
int getExpLength(TreeNode *root);

// In Order traversal function:
void inOrderTraversal(TreeNode *root, char *bufferStr, int *pos);

#endif
