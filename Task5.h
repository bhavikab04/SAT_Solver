#ifndef TASK5_H
#define TASK5_H

#include <stdbool.h>
#include "Task2.h" // Needed for the TreeNode struct definition.

/**
 * @file Task5.h
 * @brief Header for functions that evaluate logical formulas.
 *
 * This file outlines the tools for processing logical expression trees. It includes
 * functions for finding a formula's result, using variables from a file, and
 * creating a full truth table.
 */

/**
 * @def MAX_TRUTH_TABLE_VARIABLES
 * @brief A safety limit to avoid creating excessively large truth tables.
 *
 * The number of rows in a truth table grows exponentially (2^n). This limit
 * prevents the program from becoming unresponsive with too many variables.
 */
#define MAX_TRUTH_TABLE_VARIABLES 25

/**
 * @struct AssignmentHashTable
 * @brief A structure for efficiently storing variable assignments.
 *
 * This is an "opaque" type, meaning its internal details are managed in the .c file.
 * Code outside that file interacts with it through a pointer.
 */
typedef struct AssignmentHashTable AssignmentHashTable;

/**
 * @brief Evaluates the logical formula in the tree.
 *
 * It traverses the tree, applying logical operators and looking up variable
 * values in the provided `assignments` hash table to find the final result.
 *
 * @param root The top node of the formula tree.
 * @param assignments A hash table that maps variable names to their boolean values.
 * @return The final true or false result of the formula.
 */
bool evaluateTree(TreeNode *root, const AssignmentHashTable *assignments);

/**
 * @brief Evaluates a formula using variable values from a file.
 *
 * This function reads a file where each line assigns a value to a variable
 * (e.g., "p = T"). It uses these assignments to evaluate the formula tree.
 *
 * @param root The top node of the formula tree.
 * @param filename The path to the file with the variable assignments.
 * @return Returns true if evaluation was successful, false if the file could not be read.
 */
bool evaluateFromFile(TreeNode *root, const char *filename);

/**
 * @brief Gathers all unique variable names from the formula tree.
 *
 * This function scans the entire tree and creates a list of every unique
 * variable it contains. The caller is responsible for freeing the memory
 * allocated for this list.
 *
 * @param root The top node of the formula tree.
 * @param[out] literals_list_out A pointer that will be set to the new array of strings.
 * @return The number of unique variables that were found.
 */
int collectUniqueLiterals(TreeNode *root, char ***literals_list_out);

/**
 * @brief Creates and prints a complete truth table for the formula.
 *
 * This function iterates through all possible combinations of truth values
 * for the variables and prints the formula's result for each combination.
 *
 * @param root The top node of the formula tree.
 * @param literals An array containing the names of all unique variables.
 * @param count The total number of unique variables.
 * @param formula_str The formula as a string, used for printing in the header.
 */
void printTruthTable(TreeNode *root, char **literals, int count, const char *formula_str);

#endif // TASK5_H