#ifndef TASK7_H
#define TASK7_H

#include <stdbool.h>
#include "Task2.h" // Needed for the TreeNode struct definition.

/**
 * @file Task7.h
 * @brief Header for the Conjunctive Normal Form (CNF) validity checker.
 *
 * This file declares a function that can check if a formula in CNF is a
 * tautology (meaning it is always true).
 */

/**
 * @brief Checks if a CNF formula is a tautology.
 *
 * A CNF formula is a series of 'OR' clauses joined by 'AND'. The entire formula
 * is a tautology only if every individual 'OR' clause is also a tautology.
 * A clause is a tautology if it contains a variable and its negation, like (p OR ~p).
 * This function counts the valid and invalid clauses.
 *
 * @param cnf_root The root node of the CNF formula tree.
 * @param[out] valid_clauses A pointer to an integer that will hold the count of valid clauses.
 * @param[out] invalid_clauses A pointer to an integer that will hold the count of invalid clauses.
 * @return Returns true if the formula is a tautology (zero invalid clauses), and false otherwise.
 */
bool checkCNFValidity(TreeNode* cnf_root, int* valid_clauses, int* invalid_clauses);

#endif // TASK7_H