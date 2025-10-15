#include <stdio.h>
#include <stdlib.h>
#include <string.h>
#include <ctype.h>

typedef struct Node
{
    char data;
    struct Node *left;  // Points to Binary ops
    struct Node *right; // Points to all all ops
} Node;

// Operators:
int isBinaryOp(char c)
{
    return (c == '+' || c == '*' || c == '>');
}

int isUnaryOp(char c)
{
    return (c == '~');
}

/* Create a new tree node:
    -> Allocate memory to node
    -> Initialize data and child pointers to null
 */

Node *createNode(char data)
{
    // Allocate memory to the size of the node
    Node *newNode = (Node *)malloc(sizeof(Node));
    if (newNode == NULL)
    {
        perror("Memory allocation failed");
        exit(EXIT_FAILURE);
    }

    newNode->data = data;
    newNode->left = NULL;
    newNode->right = NULL;
    return newNode;
}

Node *buildParseTree(const char *prefix, int *index)
{
    // skip (so increase index) whitespaces while ensuring that it isn't the end of the string '\0'
    // isspace return type:
    // 0 for non-whitespace chars
    // non-zero for whitespace chars
    while (prefix[*index] != '\0' && isspace(prefix[*index]))
        *index++;

    // If end of the string has been reached: return null
    if (prefix[*index] == '\0')
        return NULL;

    char value = prefix[(*index)++];
    Node *root = createNode(value);

    // Recursive building function:
    if (isBinaryOp(value))
    {
        // That means it has 2 operands: so it has a left and a right child
        root->left = buildParseTree(prefix, index);
        root->right = buildParseTree(prefix, index);
    }
    else if (isUnaryOp(value))
    {
        // Only 1 operand so by convention we take it to be the right child
        root->right = buildParseTree(prefix, index);
    }

    return root;
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
