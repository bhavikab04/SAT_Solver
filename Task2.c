#include <stdio.h>
#include <stdlib.h>
#include <string.h>
#include "Task2.h" // Includes the Stack and TreeNode definitions
// Assuming push and pop functions are accessible, 
// but since we are only popping here, we only need the Stack struct 
// and the pop function implementation (which we'll define locally for completeness 
// or assume to be provided/linked).

// Re-defining helpers from Task1 for pop/isEmpty on the Stack type
// Since Task1's stack functions are not in a header, we need to redefine them
// or assume they are available from a linked object file.
// For self-containment, we'll assume the necessary Stack functions are accessible
// or re-declare/define them here. Let's assume you'll link Task1's object file,
// but for compilation, we must include the function prototypes used:

// --- Assumed Stack Function Prototypes (from Task1) ---
int isEmpty(Stack* stack);
char pop(Stack* stack);
// NOTE: Since I don't have the implementation of isEmpty/pop 
// available outside Task1.c, I must assume they are accessible.
// For the purpose of *showing* the Task2 logic, I'll proceed with calling them.

// Since the prompt provided the full stack functions, 
// let's include them here to make Task2.c compile standalone 
// if Task2.h included Stack definition.

// We must include the stack operations used by Task2's logic.
// They are NOT good candidates for a header file if they are only used in Task1's file.
// For simplicity in this answer, I will assume the `Stack` structure and
// the `pop` and `isEmpty` functions are correctly defined/linked.
// If not, you should place the implementation of `isEmpty` and `pop` here 
// or in a separate stack utility file/header.

// For now, let's proceed with the logic assuming the stack operations are available.
// If you link Task1.o, it should work, but for a standalone Task2.c,
// you would need to define/include these:
/*
int isEmpty(Stack* stack) { return stack->top == -1; }
char pop(Stack* stack) { 
    if (isEmpty(stack)) return '\0';
    return stack->data[stack->top--];
}
*/
// --- End of assumed Stack Function Prototypes ---


// --- Private Globals & Helpers ---
// static int indexPos = 0; // NO LONGER NEEDED
static TreeNode* makeNode(char value);
static bool isBinary(char c);
static bool isUnary(char c);
static void printTreeRecursiveHelper(TreeNode *root, const char *prefix, bool isLeftChild);
// The new recursive helper function for building the tree
static TreeNode* buildTreeRecursive(Stack *stack); // MODIFIED SIGNATURE


// --- Public Function Definitions ---

bool isAtom(char c) {
    // Assuming variables are any character that is NOT an operator.
    return !(isBinary(c) || isUnary(c));
}

// MODIFIED: This is now a wrapper function that takes the Stack*.
TreeNode* prefixToTree(Stack *prefix_stack) {
    if (prefix_stack == NULL || isEmpty(prefix_stack)) {
        return NULL;
    }
    // No global index reset needed. The stack naturally advances with pop.
    return buildTreeRecursive(prefix_stack); // Call the helper to do the actual work.
}

void printTreeVertical(TreeNode *root) {
    if (root == NULL) {
        printf("Tree is empty.\n");
        return;
    }
    printf("\nParse Tree Structure:\n");
    printf("%c\n", root->data);
    
    // Note: The print helper needs to be able to handle left=NULL for unary logic.
    // The original print function seemed to miss printing the left node if it existed for unary.
    // For a unary operator, convention is often to use the RIGHT child.
    // However, the original Task2.c assigned to 'right' for unary, which we keep.
    if (isUnary(root->data)) {
        // Unary operator like '~' only has one child (right in our convention)
        printTreeRecursiveHelper(root->right, "", true); 
    } else if (isBinary(root->data)) {
        // Binary operators have two children
        printTreeRecursiveHelper(root->left, "", false);
        printTreeRecursiveHelper(root->right, "", true);
    }
}

// --- Private Helper Implementations ---

// NEW: This is the actual recursive logic, now taking the Stack*.
static TreeNode* buildTreeRecursive(Stack *stack) {
    if (isEmpty(stack))
        return NULL;

    // Get the next symbol from the stack
    char ch = pop(stack);
    TreeNode *node = makeNode(ch);

    if (isBinary(ch)) {
        // Binary operators need two operands (subtrees)
        node->left = buildTreeRecursive(stack);
        node->right = buildTreeRecursive(stack);
    } else if (isUnary(ch)) {
        // Unary operators need one operand (sub-tree), conventionally right child in this scheme
        node->right = buildTreeRecursive(stack);
    }
    // Atoms (operands) automatically stop the recursion for that branch.
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

// Note on printTreeRecursiveHelper:
// The original code had a slight issue in the unary printing path: 
// printTreeRecursiveHelper(root->left, newPrefix, true); 
// was called, but 'node->left' is NULL for unary operators. 
// We should only call for the right child. The fix is applied below:

static void printTreeRecursiveHelper(TreeNode *root, const char *prefix, bool isLeftChild) {
    if (root == NULL)
        return;

    // Use isLeftChild to determine the branch drawing character
    const char *connector = isLeftChild ? "`-- " : "|-- ";
    printf("%s%s%c\n", prefix, connector, root->data);

    char newPrefix[256];
    // Extend the prefix drawing: ' ' for the `--- branch, '|' for the |-- branch
    sprintf(newPrefix, "%s%s", prefix, (isLeftChild ? "    " : "|   "));

    if (isBinary(root->data)) {
        // Binary: print left then right. Left is NOT the last child of its parent's branch.
        printTreeRecursiveHelper(root->left, newPrefix, false); 
        // Right is the last child of its parent's branch.
        printTreeRecursiveHelper(root->right, newPrefix, true);
    } else if (isUnary(root->data)) {
        // Unary: print only the right child (in our convention). It is the only child.
        printTreeRecursiveHelper(root->right, newPrefix, true);
    }
    // If it is an Atom, we stop recursion.
}