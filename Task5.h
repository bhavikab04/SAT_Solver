#ifndef TASK5_H
#define TASK5_H

#include <stdbool.h>
#include "Task2.h" // For TreeNode

// --- Configuration ---
#define MAX_TRUTH_TABLE_VARIABLES 10

// --- Opaque Pointer for the Hash Table ---
// This is the type your Task5.c file actually uses.
typedef struct AssignmentHashTable AssignmentHashTable;

// --- Function Declarations ---

/**
 * @brief Recursively evaluates the tree using a fast hash table for assignments.
 *
 * (This prototype now matches the definition in Task5.c)
 */
bool evaluateTree(TreeNode *root, const AssignmentHashTable *assignments);

/**
 * @brief Reads truth assignments from a file and evaluates the tree.
 */
bool evaluateFromFile(TreeNode *root, const char *filename);

/**
 * @brief Collects all unique literals (variables) from the tree.
 */
int collectUniqueLiterals(TreeNode *root, char ***literals_list_out);

/**
 * @brief Generates and prints the complete truth table for the formula.
 */
void printTruthTable(TreeNode *root, char **literals, int count, const char *formula_str);

#endif // TASK5_H