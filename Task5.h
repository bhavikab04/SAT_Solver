#ifndef TASK5_H
#define TASK5_H

#include <stdbool.h>
#include "Task2.h" // For TreeNode definition

/**
 * @brief Defines a mapping from a literal (string) to its boolean truth value.
 */
typedef struct {
    const char *literal;
    bool value;
} TruthAssignment;


/**
 * @brief Evaluates the truth value of the expression tree for a given set of assignments.
 * @param root The root of the parse tree.
 * @param assignments An array of TruthAssignment structs.
 * @param num_assignments The number of elements in the assignments array.
 * @return The boolean result of the evaluation.
 */
bool evaluateTree(TreeNode *root, const TruthAssignment assignments[], int num_assignments);


/**
 * @brief Traverses the tree and collects all unique literals (variables).
 * @param root The root of the parse tree.
 * @param literals_list_out A pointer to a char** which will be populated with the list of literals.
 * The caller is responsible for freeing this list and its contents.
 * @return The number of unique literals found.
 */
int collectUniqueLiterals(TreeNode *root, char ***literals_list_out);


/**
 * @brief Generates and prints a complete truth table for the given formula tree.
 * @param root The root of the parse tree.
 * @param literals An array of strings representing the unique literals in the formula.
 * @param count The number of unique literals.
 * @param formula_str A string representation of the formula to be used in the table header.
 */
void printTruthTable(TreeNode* root, char** literals, int count, const char* formula_str);


#endif // TASK5_H

