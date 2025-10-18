#ifndef TASK2_H
#define TASK2_H

#include <stdbool.h> // For the bool type

// --- Stack Structure Definition (Copied from Task1.c for dependency) ---
typedef struct {
    char* data;    // Array to hold stack data
    int top;       // Index of the top element
    int capacity;  // Max size of the stack
} Stack;

// -------------------------------
//   Public Structure for Tree Node
// -------------------------------
// The structure for a node in the parse tree.
typedef struct TreeNode {
    char data;
    struct TreeNode *left;
    struct TreeNode *right;
} TreeNode;


// -------------------------------
//   Public Function Prototypes
// -------------------------------

/**       
 * @brief Converts a prefix expression, stored in a stack, into a parse tree.
 * The stack is consumed during this process.
 * @param prefix_stack The stack containing the prefix expression. 
 *                       Popping elements yields the expression in order.
 * @return A pointer to the root node of the newly created tree.
 */
TreeNode* prefixToTree(Stack *prefix_stack); // MODIFIED SIGNATURE

/**
 * @brief Prints a visual, vertical representation of the parse tree.
 * @param root The root node of the tree to print.
 */
void printTreeVertical(TreeNode *root);

/**
 * @brief Checks if a character is an atom (a variable/operand).
 * @param c The character to check.
 * @return True if the character is an atom, false otherwise.
 */
bool isAtom(char c);


#endif // TASK2_H