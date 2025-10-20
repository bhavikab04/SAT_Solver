#ifndef TASK5_H
#define TASK5_H

#include <stdbool.h>
#include "Task2.h" // For TreeNode

// --- Configuration ---
#define MAX_TRUTH_TABLE_VARIABLES 10

// --- Opaque Pointer for the new Hash Table ---
// This is for your new functions
typedef struct AssignmentHashTable AssignmentHashTable;

// --- FIX 1: ADD THE TruthAssignment STRUCT BACK ---
// Your main.c file needs this struct definition.
typedef struct {
    char* literal;
    bool value;
} TruthAssignment;


// --- Function Declarations ---

// --- FIX 2: ADD THE OLD evaluateTree PROTOTYPE BACK ---
// Your main.c file calls this version.
bool evaluateTree(TreeNode* root, TruthAssignment* assignments, int num_assignments);


/**
 * @brief NEW: Recursively evaluates the tree using a fast hash table for assignments.
 *
 * NOTE: I have renamed this to "evaluateTreeWithHash" to avoid a
 * "conflicting types" error with the function main.c uses.
 * You must also rename the function in your Task5.c file.
 */
bool evaluateTreeWithHash(TreeNode *root, const AssignmentHashTable *assignments);

/**
 * @brief Reads truth assignments from a file and evaluates the tree.
 */
bool evaluateFromFile(TreeNode *root, const char *filename);

/**
 * @brief Collects all unique literals (variables) from the tree.
 * (This function's signature and purpose remain the same).
 */
int collectUniqueLiterals(TreeNode *root, char ***literals_list_out);

/**
 * @brief Generates and prints the complete truth table for the formula.
 *
 * (This 4-argument prototype is correct for your main.c file)
 */
void printTruthTable(TreeNode *root, char **literals, int count, const char *formula_str);

#endif // TASK5_H