#ifndef TASK7_H
#define TASK7_H

#include <stdbool.h>
#include "Task2.h" // Required for the TreeNode struct definition

/**
 * @brief Checks if a given CNF formula tree is a tautology and counts clauses.
 * @param cnf_root The root of the tree representing the formula in CNF.
 * @param valid_clauses A pointer to an integer where the count of valid (tautological) clauses will be stored.
 * @param invalid_clauses A pointer to an integer where the count of invalid clauses will be stored.
 * @return Returns true if the formula is a tautology (all clauses are valid), and false otherwise.
 */
bool checkCNFValidity(TreeNode* cnf_root, int* valid_clauses, int* invalid_clauses);

#endif // TASK7_H
