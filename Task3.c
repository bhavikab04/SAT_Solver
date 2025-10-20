/**
 * @file parse_tree_to_infix.c
 *
 * @brief Implentation of functions to construct infix expression from parse tree by in-order traversal.
 *
 * Structure of code:
 * Core functions:
 * 1) In-order traversal function to build the infix expression from the parse tree.
 *
 * Helper functions:
 * 1) Total expression length calculator function.
 * 2) Function to check if the operator is binary.
 * 3) Function to check if the operator is unary.
 *
 *
 */

#include <stdio.h>
#include <stdlib.h>
#include <string.h>
#include <stdbool.h>
#include <ctype.h>
#include "Task2.h" //for TreeNode definition, isAtom and freeTree functions
#include "Task3.h" //for getExpLength and inOrderTraversal function prototypes
/*
from task2.h:
typedef struct TreeNode {
    char data;
    struct TreeNode *left;
    struct TreeNode *right;
} TreeNode; */

// Operators:

/**
 * @brief Checks if a string token is a binary operator.
 * @param c The string token to check.
 * @return bool True if the token is a binary operator ('+', '*', '>'), false otherwise.
 */
bool isBinaryOp(const char *c)
{
    return (strcmp(c, "+") == 0 || strcmp(c, "*") == 0 || strcmp(c, ">") == 0);
}

/**
 * @brief Checks if a string token is a unary operator.
 * @param c The string token to check.
 * @return bool True if the token is a unary operator ('~'), false otherwise.
 */
bool isUnaryOp(const char *c)
{
    return (strcmp(c, "~") == 0);
}

// -------Inorder traversal---------

/**
 * @brief Performs an in-order traversal (infix) of the parse tree and writes it to a buffer.
 *
 * Structure of code:
 * -> recursively visit(left)
 * -> print(node)
 * -> recursively visit(right)
 *
 * @param root Pointer to the root of the subtree
 * @param bufferStr String buffer that updates the expression recursively to store the infix expression
 * @param position Pointer to current position in the buffer string
 */

void inOrderTraversal(TreeNode *root, char *bufferStr, int *position)
{
    if (root == NULL)
    {
        return;
    }

    // We'll come across 2 kinds of char: Operator or variable

    // if there's a binary operator, then there's a left and right node
    if (isBinaryOp(root->data))
    {
        // 1. recursively visit(left)
        bufferStr[(*position)++] = '(';
        inOrderTraversal(root->left, bufferStr, position);

        // 2. print(node) or save the node
        bufferStr[(*position)++] = ' ';

        int op_length = (int)strlen(root->data);
        memcpy(&bufferStr[*position], root->data, op_length);
        *position += op_length;

        bufferStr[(*position)++] = ' ';

        // 3. recursively visit(right)
        inOrderTraversal(root->right, bufferStr, position);
        bufferStr[(*position)++] = ')';
    }

    // If it's unary, only right side exists, so print the node first and then traverse right
    else if (isUnaryOp(root->data))
    {
        bufferStr[(*position)++] = '(';

        int op_length = (int)strlen(root->data);
        memcpy(&bufferStr[*position], root->data, op_length);
        *position += op_length;

        inOrderTraversal(root->right, bufferStr, position);
        bufferStr[(*position)++] = ')';
    }

    // If it's a variable, print it directly
    else
    {
        int atom_length = (int)strlen(root->data);
        memcpy(&bufferStr[*position], root->data, atom_length);
        *position += atom_length;
    }
}

//-------Calculation of Expression length---------
/**
 * @brief Calculates the total length of infix expression represented by the parse tree.
 * (Includes parantheses and spaces)
 * @param root Pointer to the root node of the subtree
 * @return int Total length of the expression
 */
int getExpLength(TreeNode *root)
{
    // Stop recursion condition:
    if (root == NULL)
    {
        return 0;
    }

    // Calculate the length of the data itself:
    int data_length = (int)strlen(root->data);

    // Leaf node (Operand):
    if (root->left == NULL && root->right == NULL)
    {
        return data_length;
    }

    // Recursive functions to calculate length:
    //  Unary Operator (!A): 2 chars ('(', ')') + Length(A) + Length(data)
    if (isUnaryOp(root->data))
    {
        // Unary operator uses the right child for its single operand
        return 2 + getExpLength(root->right) + data_length;
    }

    // Binary Operator ((A + B)): 4 chars ('(', ' ', OP, ' ', ')') + Length(A) + Length(B) + Length(data)
    if (isBinaryOp(root->data))
    {
        return 4 + getExpLength(root->left) + getExpLength(root->right) + data_length;
    }

    return 0;
}
