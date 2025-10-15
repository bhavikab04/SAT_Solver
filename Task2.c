#include <stdio.h>
#include <stdlib.h>
#include <stdbool.h>
#include <string.h>

// -------------------------------
//   Structure for Tree Node
// -------------------------------
typedef struct TreeNode {
    char data;
    struct TreeNode *left;
    struct TreeNode *right;
} TreeNode;

static int indexPos = 0;

// -------------------------------
//   Helper Functions
// -------------------------------
TreeNode* makeNode(char value) {
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

bool isBinary(char c) {
    return (c == '+' || c == '*' || c == '>');
}

bool isUnary(char c) {
    return (c == '!');
}

bool isAtom(char c) {
    return !(isBinary(c) || isUnary(c));
}

// -------------------------------
//   Convert Prefix → Parse Tree
// -------------------------------
TreeNode* prefixToTree(const char *expr) {
    if (expr[indexPos] == '\0')
        return NULL;

    char ch = expr[indexPos++];
    TreeNode *node = makeNode(ch);

    if (isBinary(ch)) {
        node->left = prefixToTree(expr);
        node->right = prefixToTree(expr);
    } else if (isUnary(ch)) {
        node->left = prefixToTree(expr);
    }
    return node;
}

// -------------------------------
//   Print Tree (Rooted Vertical View)
// -------------------------------
void printTreeVertical(TreeNode *root, const char *prefix, bool isLeftChild) {
    if (root == NULL)
        return;

    const char *connector = isLeftChild ? "`-- " : "|-- ";
    printf("%s%s%c\n", prefix, connector, root->data);

    char newPrefix[256];
    sprintf(newPrefix, "%s%s", prefix, (isLeftChild ? "    " : "|   "));

    if (isBinary(root->data)) {
        printTreeVertical(root->left, newPrefix, false);
        printTreeVertical(root->right, newPrefix, true);
    } else if (isUnary(root->data)) {
        printTreeVertical(root->left, newPrefix, true);
    }
}

// -------------------------------
//   Main Function (Takes Input)
// -------------------------------
int main() {
    char prefix_expr[100];

    printf("Enter a prefix expression: ");
    scanf("%s", prefix_expr);

    indexPos = 0;

    TreeNode *root = prefixToTree(prefix_expr);
    if (!root) {
        printf("Tree construction failed.\n");
        return 0;
    }

    printf("\n--- Prefix to Rooted Binary Parse Tree ---\n");
    printf("Input Prefix: %s\n", prefix_expr);
    printf("Root Node: %c\n", root->data);
    printf("\nParse Tree Structure (Rooted View):\n");

    printf("%c\n", root->data);
    char prefix[] = "";

    if (isUnary(root->data)) {
        printTreeVertical(root->left, prefix, true);
    } else if (isBinary(root->data)) {
        printTreeVertical(root->left, prefix, false);
        printTreeVertical(root->right, prefix, true);
    }

    return 0;
}