#ifndef TASK3_H
#define TASK3_H

typedef struct Node
{
    char data;
    struct Node *left;  // Points to Binary ops
    struct Node *right; // Points to all all ops
} Node;

//Expression length calculator function:
int getExpLength(Node* root);

//In Order traversal function:
void inOrderTraversal(Node* root, char* bufferStr, int* pos);

//Free all memory allocated to parse tree (using post-order traversal):
void freeTree(Node* root);

#endif
