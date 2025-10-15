#ifndef TASK3_H
#define TASK3_H

typedef struct TreeNode
{
    char data;
    struct TreeNode *left;  // Points to Binary ops
    struct TreeNode *right; // Points to all all ops
} TreeNode;

//Expression length calculator function:
int getExpLength(TreeNode* root);

//In Order traversal function:
void inOrderTraversal(TreeNode* root, char* bufferStr, int* pos);

//Free all memory allocated to parse tree (using post-order traversal):
void freeTree(TreeNode* root);

#endif
