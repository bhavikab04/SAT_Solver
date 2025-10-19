#ifndef TASK5_H
#define TASK5_H

#include <stdbool.h>
#include "Task2.h" // Assumes TreeNode->data is now char* value

// Updated to handle string-based literals
typedef struct {
    char* literal; // The name of the literal (e.g., "p", "raining_today")
    bool value;    // The assigned truth value (true or false)
} TruthAssignment;

/**
 * @brief Finds all unique literals in an expression tree using a high-performance hash table.
 * @param node The root of the tree to search.
 * @param literals_list A pointer to an array of strings that will be allocated and filled.
 * @return The number of unique literals found.
 */
int collectUniqueLiterals(TreeNode* node, char*** literals_list);

/**
 * @brief Prompts the user to enter truth assignments from the terminal for string-based literals.
 * @param literals An array of unique literal names.
 * @param count The number of literals in the array.
 * @return A dynamically allocated array of TruthAssignment structs.
 */
TruthAssignment* getAssignmentsFromTerminal(char** literals, int count);

/**
 * @brief Reads truth assignments for string-based literals from a specified file.
 * @param filename The path to the file.
 * @param num_assignments A pointer to an integer to store the number of assignments read.
 * @return A dynamically allocated array of TruthAssignment structs, or NULL on failure.
 */
TruthAssignment* getAssignmentsFromFile(const char* filename, int* num_assignments);

/**
 * @brief Evaluates an expression tree with string-based literals.
 * @param root The root of the expression tree.
 * @param assignments An array of TruthAssignment structs.
 * @param num_assignments The number of assignments in the array.
 * @return The final boolean result of the evaluation.
 */
bool evaluateTree(TreeNode* root, const TruthAssignment assignments[], int num_assignments);

/**
 * @brief Generates and prints the complete truth table for the formula represented by the tree.
 * @param root The root of the expression tree.
 * @param literals An array of unique literal names.
 * @param count The number of unique literals.
 * @param formula_str The string representation of the formula for the table header.
 */
void printTruthTable(TreeNode* root, char** literals, int count, const char* formula_str);

#endif // TASK5_H