#ifndef TASK3_H
#define TASK3_H

/**
 * @file Task3.h
 * @brief Header file for Task 3: Conversion of prase tree to infix.
 *
 * Declares functions to convert the parse tree back into an
 * infix expression string.
 *
 * @note This header requires the `TreeNode` structure definition,
 * which is defined in task2.h itself.
 */


/**
 * @brief Calculates the required string length for the infix expression.
 *
 * Traverses the tree to determine the exact buffer size needed to store
 * the infix representation, including all operands, operators,
 * and required parentheses.
 *
 * @param root The root of the parse tree.
 * @return The total length /no. of characters of the infix string.
 */
int getExpLength(TreeNode* root);

/**
 * @brief Reconstructs the infix expression string using in-order traversal.
 *
 * Recursively visits the tree nodes in-order (left, root, right) and
 * writes the corresponding characters (operands, operators, parentheses)
 * into the provided buffer.
 *
 * @param root The current node being visited in the tree.
 * @param bufferStr A pointer to the character buffer to write into.
 * @param pos A pointer to an integer tracking the current write
 * position (index) in the `bufferStr`.
 */
void inOrderTraversal(TreeNode* root, char* bufferStr, int* pos);

/**
 * @brief Frees all memory allocated to the parse tree.
 *
 * Recursively frees all nodes and their associated string data
 * using a post-order traversal.
 *
 * @note This function is typically defined in Task 2 (task2.c)
 * and re-declared here for use by Task 3.
 * @param root The root of the tree (or subtree) to be freed.
 */
void freeTree(TreeNode* root);

#endif 