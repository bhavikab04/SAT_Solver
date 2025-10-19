#ifndef TASK2_H
#define TASK2_H

#include <stdbool.h> // For the bool type

// --- Stack Structure Definition (Canonical for the project) ---
// Note: This must be the ONLY definition of struct Stack/Stack in your entire project.
typedef struct Stack {
    char** data;      // Array to hold stack data (array of char pointers/strings)
    int top;          // Index of the top element
    int capacity;     // Max size of the stack
} Stack; // The typedef creates an alias 'Stack' for 'struct Stack'

// -------------------------------
//  Public Structure for Tree Node
// -------------------------------
// The structure for a node in the parse tree.
typedef struct TreeNode {
    char *data; // CHANGED from char to char* to store string tokens
    struct TreeNode *left;
    struct TreeNode *right;
} TreeNode;


// -------------------------------
//  Public Function Prototypes
// -------------------------------

/** * @brief Converts a prefix expression, stored in a stack of string tokens, into a parse tree.
 * The stack is consumed during this process.
 * @param prefix_stack The stack containing the prefix expression string tokens. 
 * Popping elements yields the expression in order.
 * @return A pointer to the root node of the newly created tree.
 */
TreeNode* prefixToTree(Stack *prefix_stack); // SIGNATURE UNCHANGED, but behavior changes

/**
 * @brief Prints a visual, vertical representation of the parse tree.
 * @param root The root node of the tree to print.
 */
void printTreeVertical(TreeNode *root);

/**
 * @brief Frees all memory allocated for the parse tree nodes and their data strings.
 * @param root The root node of the tree to free.
 */
void freeTree(TreeNode *root);

/**
 * @brief Checks if a string token represents an atom (a variable/operand).
 * Checks the FIRST character for simplicity, assuming operators are single-character.
 * @param token The string token to check.
 * @return True if the token is an atom, false otherwise.
 */
bool isAtom(const char *token);


#endif // TASK2_H