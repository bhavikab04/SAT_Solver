#ifndef TASK2_H
#define TASK2_H

#include <stdbool.h> // For the bool type

// -------------------------------
//   Public Structure for Tree Node
// -------------------------------
// The structure for a node in the parse tree.
typedef struct TreeNode {
    char data;
    struct TreeNode *left;
    struct TreeNode *right;
} TreeNode;


// -------------------------------
//   Public Function Prototypes
// -------------------------------

/**       
 * @brief Converts a prefix expression string into a parse tree.
 * @param expr The null-terminated prefix expression string.
 * @return A pointer to the root node of the newly created tree.
 */
TreeNode* prefixToTree(const char *expr);

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

