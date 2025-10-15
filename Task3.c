#include <stdio.h>
#include <stdlib.h>
#include <string.h>
#include <ctype.h>

/* typedef struct Node
{
    char data;
    struct Node *left;  // Points to Binary ops
    struct Node *right; // Points to all all ops
} Node; */

// Operators:
int isBinaryOp(char c)
{
    return (c == '+' || c == '*' || c == '>');
}

int isUnaryOp(char c)
{
    return (c == '~');
}

// Inorder traversal:
/* Structure:
    -> recursively visit(left)
    -> print(node)
    -> recursively visit(right)
 */
/*
-> pointer root: points to root of subtree
-> bufferStr: a string buffer that updates the expression recursively. Will contain final infix expression
-> position: gives current position in the buffer string.
 */
void inOrderTraversal(Node *root, char *bufferStr, int *position)
{
    if (root == NULL)
    {
        return;
    }

    // We'll come across 2 kinds of char: Operator or variable

    // if there's a binary operator, then there's a left and right node
    if (isBinaryOp(root->data))
    {
        // recursively visit(left)
        bufferStr[(*position)++] = '(';
        inOrderTraversal(root->left, bufferStr, position);

        // print(node) or save the node
        bufferStr[(*position)++] = ' ';
        bufferStr[(*position)++] = root->data;
        bufferStr[(*position)++] = ' ';

        // recursively visit(right)
        inOrderTraversal(root->right, bufferStr, position);
        bufferStr[(*position)++] = ')';
    }

    // If it's unary, only right side exists, so print the node first and then traverse right
    else if (isUnaryOp(root->data))
    {
        bufferStr[(*position)++] = '(';
        bufferStr[(*position)++] = root->data;

        inOrderTraversal(root->right, bufferStr, position);
        bufferStr[(*position)++] = ')';
    }

    // If it's a variable, print it directly
    else
    {
        bufferStr[(*position)++] = root->data;
    }
}

int getExpLength(Node* root) {
    //Stop recursion condition:
    if (root == NULL) {
        return 0;
    }

    // Leaf node (Operand): 1 char
    if (root->left == NULL && root->right == NULL) {
        return 1; 
    }

    //Recursive functions to calculate length:
    // Unary Operator (!A): 2 chars ('(', '!', ')') + Length(A)
    if (isUnaryOp(root->data)) {
        // Unary operator uses the right child for its single operand
        return 2 + getExpLength(root->right); 
    }

    // Binary Operator ((A + B)): 4 chars ('(', ' ', OP, ' ', ')') + Length(A) + Length(B)
    if (isBinaryOp(root->data)) {
        return 4 + getExpLength(root->left) + getExpnLength(root->right);
    }
    
    return 0;
}

//To optimize the code further: free the allocated memory for tree (using post order traversal)
void freeTree(Node *root)
{
    if (root == NULL)
    {
        return;
    }
    freeTree(root->left);
    freeTree(root->right);
    free(root);
}

