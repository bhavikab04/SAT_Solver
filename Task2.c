#include <stdio.h>
#include <stdlib.h>
#include <string.h>
#include "Task2.h" 

// --- Assumed Stack Function Prototypes (Updated for char* tokens) ---
// NOTE: These MUST be implemented elsewhere (e.g., Task1.c/utils.c) to handle char*
int isEmpty(Stack* stack);
char* pop(Stack* stack); // ASSUMED: pop now returns a char*
void freeStack(Stack *stack); // ASSUMED: frees the stack structure AND the tokens it holds

// --- Private Globals & Helpers ---
static TreeNode* makeNode(const char *value);
static bool isBinary(const char *token);
static bool isUnary(const char *token);
static void printTreeRecursiveHelper(TreeNode *root, const char *prefix, bool isLeftChild);
static TreeNode* buildTreeRecursive(Stack *stack); 

// --- Public Function Definitions ---

// Updated to take a string token
bool isAtom(const char *token) {
    if (!token || token[0] == '\0') return false;
    // Check the first character against known operators
    return !(isBinary(token) || isUnary(token));
}

TreeNode* prefixToTree(Stack *prefix_stack) {
    if (prefix_stack == NULL || isEmpty(prefix_stack)) {
        return NULL;
    }
    return buildTreeRecursive(prefix_stack);
}

void printTreeVertical(TreeNode *root) {
    if (root == NULL) {
        printf("Tree is empty.\n");
        return;
    }
    printf("\nParse Tree Structure:\n");
    printf("%s\n", root->data);
    
    // The print helper needs to be called correctly for the children
    if (isUnary(root->data)) {
        printTreeRecursiveHelper(root->right, "", true); 
    } else if (isBinary(root->data)) {
        // Left is handled as NOT the last child (false for isLeftChild) 
        // in the branch rendering logic.
        printTreeRecursiveHelper(root->left, "", false); 
        // Right is handled as the last child (true for isLeftChild)
        printTreeRecursiveHelper(root->right, "", true);
    }
}

void freeTree(TreeNode *root) {
    if (root == NULL) {
        return;
    }

    // Recursively free children
    freeTree(root->left);
    freeTree(root->right);

    // Free the dynamically allocated data string
    if (root->data != NULL) {
        free(root->data);
    }

    // Free the node itself
    free(root);
}

// --- Private Helper Implementations ---

static TreeNode* buildTreeRecursive(Stack *stack) {
    if (isEmpty(stack))
        return NULL;

    // Get the next token (string) from the stack
    char *token = pop(stack); // pop() returns char*
    if (!token) return NULL; // Should not happen if isEmpty is checked, but for safety

    TreeNode *node = makeNode(token);
    // The token string from the stack is now copied/stored in the node.
    // The original token from the stack must be freed if it was dynamically allocated 
    // when pushed. For now, we assume `pop` handles token memory or that 
    // `makeNode` takes ownership and frees it (but standard is to copy).
    // Let's assume `pop` gives us a string that MUST be freed by the caller 
    // after it's no longer needed (i.e., after copying to the node).

    // Since makeNode *copies* the string, we free the token from the stack now.
    free(token); 
    
    if (isBinary(node->data)) {
        // Binary operators need two operands (subtrees)
        // NOTE: Order of recursion is important for prefix: Left then Right.
        node->left = buildTreeRecursive(stack);
        node->right = buildTreeRecursive(stack);
    } else if (isUnary(node->data)) {
        // Unary operators need one operand (sub-tree), conventionally right child
        node->right = buildTreeRecursive(stack);
    }
    // Atoms (operands) automatically stop the recursion for that branch.
    return node;
}


static TreeNode* makeNode(const char *value) {
    TreeNode* node = (TreeNode*)malloc(sizeof(TreeNode));
    if (!node) {
        fprintf(stderr, "Memory allocation failed for TreeNode.\n");
        exit(EXIT_FAILURE);
    }
    
    // Allocate memory for the string data
    node->data = strdup(value); // strdup() is a convenient way to allocate and copy
    if (!node->data) {
        fprintf(stderr, "Memory allocation failed for TreeNode data.\n");
        free(node);
        exit(EXIT_FAILURE);
    }

    node->left = NULL;
    node->right = NULL;
    return node;
}

// Operators are single characters, so we check the first character of the token
static bool isBinary(const char *token) {
    if (!token || strlen(token) != 1) return false;
    char c = token[0];
    return (c == '+' || c == '*' || c == '>');
}

static bool isUnary(const char *token) {
    if (!token || strlen(token) != 1) return false;
    char c = token[0];
    return (c == '~');
}

// Helper to print the tree structure visually
static void printTreeRecursiveHelper(TreeNode *root, const char *prefix, bool isLeftChild) {
    if (root == NULL)
        return;

    // Use isLeftChild to determine the branch drawing character
    const char *connector = isLeftChild ? "`-- " : "|-- ";
    printf("%s%s%s\n", prefix, connector, root->data); // Print the full string

    char newPrefix[256];
    // Extend the prefix drawing: ' ' for the `--- branch, '|' for the |-- branch
    // Note: The newPrefix calculation is a bit of a trick for visual representation
    sprintf(newPrefix, "%s%s", prefix, (isLeftChild ? "    " : "|   "));

    if (isBinary(root->data)) {
        // Binary: print left then right.
        printTreeRecursiveHelper(root->left, newPrefix, false); 
        printTreeRecursiveHelper(root->right, newPrefix, true);
    } else if (isUnary(root->data)) {
        // Unary: print only the right child (it is the only child, so it's the 'last')
        printTreeRecursiveHelper(root->right, newPrefix, true);
    }
}