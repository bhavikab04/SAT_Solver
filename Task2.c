#include <stdio.h>
#include <stdlib.h>
#include <string.h>
#include "Task2.h" 
#include "Task1.h" // <-- IMPORTANT: Include this for Stack, pop(), isEmpty()

// --- Private Helper Prototypes ---
static TreeNode* makeNode(const char *value);
static bool isBinary(const char *token);
static bool isUnary(const char *token);
static void printTreeRecursiveHelper(TreeNode *root, const char *prefix, bool isLastChild);

// === NEW: A stack for TreeNodes, to avoid recursion ===
typedef struct {
    TreeNode** data;
    int top;
    int capacity;
} NodeStack;

static NodeStack* createNodeStack(int capacity) {
    NodeStack* ns = (NodeStack*)malloc(sizeof(NodeStack));
    if (!ns) exit(1);
    ns->data = (TreeNode**)malloc(sizeof(TreeNode*) * capacity);
    if (!ns->data) exit(1);
    ns->capacity = capacity;
    ns->top = -1;
    return ns;
}
static int isNodeStackEmpty(NodeStack* ns) { return ns->top == -1; }
static void node_push(NodeStack* ns, TreeNode* node) { ns->data[++ns->top] = node; }
static TreeNode* node_pop(NodeStack* ns) { return ns->data[ns->top--]; }
static void freeNodeStack(NodeStack* ns) { free(ns->data); free(ns); }
// === END: New NodeStack ===


// --- Public Function Definitions ---

bool isAtom(const char *token) {
    if (!token || token[0] == '\0') return false;
    return !(isBinary(token) || isUnary(token));
}

/**
 * @brief NEW: Iterative (non-recursive) prefix-to-tree converter.
 * This function handles deep trees without causing a stack overflow.
 */
TreeNode* prefixToTree(Stack *prefix_stack) {
    if (prefix_stack == NULL || isEmpty(prefix_stack)) {
        return NULL;
    }

    // 1. Get the first token: this must be the root.
    char* token = pop(prefix_stack);
    if (!token) return NULL;
    
    TreeNode* root = makeNode(token);
    free(token); // makeNode copies it

    // 2. Create a node_stack to manage which nodes need children
    NodeStack* node_stack = createNodeStack(prefix_stack->capacity);
    node_push(node_stack, root);

    // 3. Loop while there are nodes waiting for children
    while (!isNodeStackEmpty(node_stack)) {
        TreeNode* current = node_pop(node_stack);

        // 4. Check what kind of node it is and attach its children
        if (isUnary(current->data)) {
            // --- Unary Operator (~) ---
            // It only needs a right child.
            token = pop(prefix_stack);
            if (!token) { /* Error: stack empty */ break; }
            current->right = makeNode(token);
            free(token);
            // Push right child so it gets processed
            node_push(node_stack, current->right);

        } else if (isBinary(current->data)) {
            // --- Binary Operator (*, +, >) ---
            // It needs a left child and a right child.
            // In prefix, the order is (Op, Left, Right).
            // We must push RIGHT then LEFT so that LEFT is processed first.
            
            // Add RIGHT child
            token = pop(prefix_stack);
            if (!token) { /* Error: stack empty */ break; }
            current->right = makeNode(token);
            free(token);
            // Push right child so it gets processed
            node_push(node_stack, current->right); 

            // Add LEFT child
            token = pop(prefix_stack);
            if (!token) { /* Error: stack empty */ break; }
            current->left = makeNode(token);
            free(token);
            // Push left child so it gets processed
            node_push(node_stack, current->left); 
        }
        // else: It's an Atom (operand), so it's a leaf.
        // We do nothing, and the loop continues.
    }

    // 5. Clean up and return
    freeNodeStack(node_stack);
    return root;
}


void printTreeVertical(TreeNode *root) {
    if (root == NULL) {
        printf("Tree is empty.\n");
        return;
    }
    printf("\nParse Tree Structure:\n");
    printf("%s\n", root->data);
    
    if (isUnary(root->data)) {
        printTreeRecursiveHelper(root->right, "", true); 
    } else {
        printTreeRecursiveHelper(root->left, "", false); 
        printTreeRecursiveHelper(root->right, "", true);
    }
}

void freeTree(TreeNode *root) {
    if (root == NULL) {
        return;
    }
    freeTree(root->left);
    freeTree(root->right);
    free(root->data);
    free(root);
}

// --- Private Helper Implementations ---

static TreeNode* makeNode(const char *value) {
    TreeNode* node = (TreeNode*)malloc(sizeof(TreeNode));
    if (!node) {
        fprintf(stderr, "Memory allocation failed for TreeNode.\n");
        exit(EXIT_FAILURE);
    }
    
    node->data = strdup(value); // strdup() allocates and copies
    if (!node->data) {
        fprintf(stderr, "Memory allocation failed for TreeNode data.\n");
        free(node);
        exit(EXIT_FAILURE);
    }

    node->left = NULL;
    node->right = NULL;
    return node;
}

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

static void printTreeRecursiveHelper(TreeNode *root, const char *prefix, bool isLastChild) {
    if (root == NULL)
        return;

    const char *connector = isLastChild ? "`-- " : "|-- ";
    printf("%s%s%s\n", prefix, connector, root->data);

    char newPrefix[512]; // Increased buffer for deep trees
    snprintf(newPrefix, sizeof(newPrefix), "%s%s", prefix, (isLastChild ? "    " : "|   "));

    if (isUnary(root->data)) {
        printTreeRecursiveHelper(root->right, newPrefix, true);
    } else {
        printTreeRecursiveHelper(root->left, newPrefix, false); 
        printTreeRecursiveHelper(root->right, newPrefix, true);
    }
}