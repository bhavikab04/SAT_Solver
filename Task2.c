#include <stdio.h>
#include <stdlib.h>
#include <string.h>
#include "Task2.h"

// --- Private Globals & Helpers ---
static int indexPos = 0;
static TreeNode* makeNode(char value);
static bool isBinary(char c);
static bool isUnary(char c);
static void printTreeRecursiveHelper(TreeNode *root, const char *prefix, bool isLeftChild);
// The new recursive helper function for building the tree
static TreeNode* buildTreeRecursive(const char *expr);


// --- Public Function Definitions ---

bool isAtom(char c) {
    return !(isBinary(c) || isUnary(c));
}

// MODIFIED: This is now a wrapper function.
// It resets the state and calls the recursive helper.
TreeNode* prefixToTree(const char *expr) {
    indexPos = 0; // Reset the global index ONCE before starting.
    return buildTreeRecursive(expr); // Call the helper to do the actual work.
}

void printTreeVertical(TreeNode *root) {
    if (root == NULL) {
        printf("Tree is empty.\n");
        return;
    }
    printf("\nParse Tree Structure:\n");
    printf("%c\n", root->data);
    if (isUnary(root->data)) {
        printTreeRecursiveHelper(root->right, "", true);
    } else if (isBinary(root->data)) {
        printTreeRecursiveHelper(root->left, "", false);
        printTreeRecursiveHelper(root->right, "", true);
    }
}

// --- Private Helper Implementations ---

// NEW: This is the actual recursive logic, moved from the original function.
// It does NOT reset the index, allowing it to make progress.
static TreeNode* buildTreeRecursive(const char *expr) {
    if (expr[indexPos] == '\0')
        return NULL;

    char ch = expr[indexPos++];
    TreeNode *node = makeNode(ch);

    if (isBinary(ch)) {
        node->left = buildTreeRecursive(expr);
        node->right = buildTreeRecursive(expr);
    } else if (isUnary(ch)) {
        node->right = buildTreeRecursive(expr);
    }
    return node;
}


static TreeNode* makeNode(char value) {
    TreeNode* node = (TreeNode*)malloc(sizeof(TreeNode));
    if (!node) {
        fprintf(stderr, "Memory allocation failed.\n");
        exit(EXIT_FAILURE);
    }
    node->data = value;
    node->left = NULL;
    node->right = NULL;
    return node;
}

static bool isBinary(char c) {
    return (c == '+' || c == '*' || c == '>');
}

static bool isUnary(char c) {
    return (c == '~');
}

static void printTreeRecursiveHelper(TreeNode *root, const char *prefix, bool isLeftChild) {
    if (root == NULL)
        return;

    const char *connector = isLeftChild ? "`-- " : "|-- ";
    printf("%s%s%c\n", prefix, connector, root->data);

    char newPrefix[256];
    sprintf(newPrefix, "%s%s", prefix, (isLeftChild ? "    " : "|   "));

    if (isBinary(root->data)) {
        printTreeRecursiveHelper(root->left, newPrefix, false);
        printTreeRecursiveHelper(root->right, newPrefix, true);
    } else if (isUnary(root->data)) {
        printTreeRecursiveHelper(root->left, newPrefix, true);
    }
}

