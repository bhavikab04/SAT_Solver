#include <stdio.h>
#include <stdlib.h>
#include <string.h>
#include "Task2.h" 
#include "Task1.h" // For Stack, pop(), isEmpty()

// --- Private Helper Prototypes ---
static TreeNode* makeNode(const char *value);
static bool isBinary(const char *token);
static bool isUnary(const char *token);
static void printTreeRecursiveHelper(TreeNode *root, const char *prefix, bool isLastChild);

// === Stack for TreeNodes (used by iterative builder) ===
typedef struct {
    TreeNode** data;
    int top;
    int capacity;
} NodeStack;

static NodeStack* createNodeStack(int capacity) {
    NodeStack* ns = (NodeStack*)malloc(sizeof(NodeStack));
    if (!ns) { fprintf(stderr, "Failed to allocate NodeStack\n"); exit(1); }
    ns->data = (TreeNode**)malloc(sizeof(TreeNode*) * capacity);
    if (!ns->data) { fprintf(stderr, "Failed to allocate NodeStack data\n"); free(ns); exit(1); }
    ns->capacity = capacity;
    ns->top = -1;
    return ns;
}
static int isNodeStackEmpty(NodeStack* ns) { return ns->top == -1; }
// Peek function is useful for the new algorithm
static TreeNode* node_peek(NodeStack* ns) { return (ns->top >= 0) ? ns->data[ns->top] : NULL; } 
static void node_push(NodeStack* ns, TreeNode* node) { ns->data[++ns->top] = node; }
static TreeNode* node_pop(NodeStack* ns) { return (ns->top >= 0) ? ns->data[ns->top--] : NULL; }
static void freeNodeStack(NodeStack* ns) { free(ns->data); free(ns); }
// === END: NodeStack ===


// --- Public Function Definitions ---

// Helper to check if a token is an atom (operand)
bool isAtom(const char *token) {
    if (!token || token[0] == '\0') return false;
    // It's an atom if it's NOT a known operator
    return !(isBinary(token) || isUnary(token));
}

/**
 * @brief Iterative prefix-to-tree converter (Revised Algorithm).
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

    // Node stack stores parent nodes waiting for children
    NodeStack* node_stack = createNodeStack(prefix_stack->capacity);

    // If root is an operator, push it, it needs children
    if (!isAtom(root->data)) {
        node_push(node_stack, root);
    }

    // 2. Process remaining tokens from the prefix stack
    while (!isEmpty(prefix_stack)) {
        // Get the parent node that's waiting for its next child
        TreeNode* current_parent = node_peek(node_stack);
        if (current_parent == NULL) {
             fprintf(stderr, "Error: Node stack empty but prefix stack has tokens (Malformed expression? Too many operands?).\n");
             while ((token = pop(prefix_stack)) != NULL) free(token); // Cleanup remaining tokens
             break; // Exit loop
        }

        // Get the next token from the input
        token = pop(prefix_stack);
        if (!token) break; // Should not happen if loop condition is correct
        TreeNode* new_node = makeNode(token);
        free(token);

        // Attach new_node to parent based on parent type and current children
        bool attached = false;
        if (isUnary(current_parent->data)) {
            // Unary operator only gets a right child
            if (current_parent->right == NULL) {
                current_parent->right = new_node;
                node_pop(node_stack); // Unary parent is now complete
                attached = true;
            } else {
                 fprintf(stderr, "Error logic: Unary node '%s' trying to attach second child '%s'?\n", current_parent->data, new_node->data);
                 freeTree(new_node); break; // Error state
            }
        } else if (isBinary(current_parent->data)) {
            // Binary operator gets left then right
            if (current_parent->left == NULL) {
                current_parent->left = new_node;
                attached = true;
                // Don't pop parent yet, it still needs a right child
            } else if (current_parent->right == NULL) {
                current_parent->right = new_node;
                node_pop(node_stack); // Binary parent is now complete
                attached = true;
            } else {
                 fprintf(stderr, "Error logic: Binary node '%s' trying to attach third child '%s'?\n", current_parent->data, new_node->data);
                 freeTree(new_node); break; // Error state
            }
        } else {
             // This should not happen if node_stack only contains operators
             fprintf(stderr, "Error logic: Atom node '%s' on node_stack?\n", current_parent->data);
             freeTree(new_node); break; // Error state
        }

        // If the new node is an operator, push it onto the stack
        // because it will need children too.
        if (attached && !isAtom(new_node->data)) {
            node_push(node_stack, new_node);
        } else if (!attached) {
            // If we somehow didn't attach (error case), free the new node
            freeTree(new_node);
        }
    }

    // 3. Final Checks and Cleanup
    if (!isNodeStackEmpty(node_stack)) {
         fprintf(stderr, "Warning: Node stack not empty after processing all tokens (Malformed expression? Operators missing operands?).\n");
         // Tree might be incomplete. You could potentially free nodes still on the stack.
         // For simplicity, we'll just free the stack structure itself.
    }
     if (!isEmpty(prefix_stack)) {
         fprintf(stderr, "Warning: Prefix stack not empty after processing (Malformed expression? Too many operands?).\n");
         while ((token = pop(prefix_stack)) != NULL) free(token); // Cleanup remaining tokens
     }

    freeNodeStack(node_stack);
    return root;
}


// --- Functions below remain the same ---

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
    free(root->data); // Free the duplicated string
    free(root);       // Free the node itself
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
    // Ensure null termination even if prefix is long
    snprintf(newPrefix, sizeof(newPrefix)-1, "%s%s", prefix, (isLastChild ? "    " : "|   ")); 
    newPrefix[sizeof(newPrefix)-1] = '\0'; // Guarantee null termination

    // Recursively print children
    if (isUnary(root->data)) {
        // Unary node only has a right child
        printTreeRecursiveHelper(root->right, newPrefix, true); // Treat it as the 'last' child
    } else {
        // Binary node or Atom (Atom's children will be NULL)
        printTreeRecursiveHelper(root->left, newPrefix, false); // Left child is not the last
        printTreeRecursiveHelper(root->right, newPrefix, true); // Right child is the last
    }
}