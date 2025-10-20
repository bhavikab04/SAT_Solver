#ifndef TASK5_H
#define TASK5_H

#include <stdbool.h>
#include "Task2.h" // For TreeNode

// --- Configuration ---
#define MAX_TRUTH_TABLE_VARIABLES 10

// --- Opaque Pointer for the new Hash Table ---
// The implementation details are hidden in Task5.c
typedef struct AssignmentHashTable AssignmentHashTable;

// --- Function Declarations ---

/**
 * @brief NEW: Recursively evaluates the tree using a fast hash table for assignments.
 *
 * @param root The root of the expression tree.
 * @param assignments A pointer to the hash table containing truth assignments.
 * @return The boolean result of the evaluation.
 */
bool evaluateTree(TreeNode *root, const AssignmentHashTable *assignments);

/**
 * @brief Reads truth assignments from a file and evaluates the tree. This function
 * now builds an internal hash table for efficient evaluation.
 *
 * @param root The root of the expression tree to evaluate.
 * @param filename The path to the file containing assignments (e.g., "p = T").
 * @return true if evaluation was successful, false on error.
 */
bool evaluateFromFile(TreeNode *root, const char *filename);

/**
 * @brief Collects all unique literals (variables) from the tree.
 * (This function's signature and purpose remain the same).
 *
 * @param root The root of the expression tree.
 * @param literals_list_out A pointer to a char** that will be populated.
 * @return The number of unique literals found.
 */
int collectUniqueLiterals(TreeNode *root, char ***literals_list_out);

/**
 * @brief Generates and prints the complete truth table for the formula.
 *
 * @param root The root of the expression tree.
 * @param literals An array of unique literal names.
 * @param count The number of unique literals.
 * @param formula_str The original infix formula string.
 */
void printTruthTable(TreeNode *root, char **literals, int count, const char *formula_str);

#endif // TASK5_H

