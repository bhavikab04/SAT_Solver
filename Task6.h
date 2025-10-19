/**
 * @file Task6.h
 * @brief Header file containing function prototypes for converting propositional logic formulas to Conjunctive Normal Form (CNF).
 *
 * This file declares the functions necessary for the three main steps of CNF conversion:
 * 1. Implication Elimination (IMPL_FREE)
 * 2. Negation Normal Form conversion (NNF)
 * 3. Distribution to achieve CNF (DISTR, CNF)
 *
 * It relies on structures and functions defined in Task2.h, Task3.h, and Task5.h.
 */
#ifndef TASK6_H
#define TASK6_H

#include <stdio.h>
#include <stdlib.h>
#include <string.h>

#include "Task2.h" //need it for struct and isAtom() function
#include "Task3.h" //need it for freeTree() function

// Core Algorithms:
/**
 * @brief Eliminates all implication operators ('>') from a formula.
 *
 * Implements the rule $\psi \rightarrow \eta \equiv \sim\psi \lor \eta$ recursively.
 *
 * @param phi The root of the formula tree.
 * @return TreeNode* A pointer to the root of the new implication-free formula tree.
 */
TreeNode *IMPL_FREE(TreeNode *phi);

/**
 * @brief Converts an implication-free formula to Negation Normal Form (NNF).
 *
 * Pushes negations inward using Double Negation and De Morgan's laws until negations
 * only appear immediately before atoms (literals).
 *
 * @param phi The root of the formula tree (must be implication-free).
 * @return TreeNode* A pointer to the root of the new formula tree in NNF.
 */
TreeNode *NNF(TreeNode *phi);

/**
 * @brief Distributes disjunction ('+') over conjunction ('*').
 *
 * Implements the distributive law $A \lor (B \land C) \equiv (A \lor B) \land (A \lor C)$.
 * Used as a sub-routine in the CNF conversion step.
 *
 * @param eta1 The left sub-formula of the disjunction.
 * @param eta2 The right sub-formula of the disjunction.
 * @return TreeNode* A pointer to the root of the new formula tree after distribution.
 */
TreeNode *DISTR(TreeNode *eta1, TreeNode *eta2);

/**
 * @brief Main function to convert an NNF formula to CNF.
 *
 * Recursively ensures the formula is a conjunction of disjunctions of literals
 * by applying the distributive function @ref DISTR.
 *
 * @param phi The root of the formula tree (must be in NNF).
 * @return TreeNode* A pointer to the root of the new formula tree in CNF.
 */
TreeNode *CNF(TreeNode *phi);

/**
 * @brief Driver function to execute the full CNF conversion process.
 *
 * Performs the sequence: IMPL_FREE $\rightarrow$ NNF $\rightarrow$ CNF.
 *
 * @param phi The root of the initial formula tree.
 * @return TreeNode* A pointer to the root of the final formula tree in CNF.
 */
TreeNode *CNF_FORMULA(TreeNode *phi);

/**
 * @brief Creates a deep copy of a propositional logic formula tree.
 *
 * Recursively duplicates the entire tree structure rooted at the input node.
 *
 * @param root A constant pointer to the root of the tree to be copied.
 * @return TreeNode* A pointer to the root of the newly created deep copy, or NULL if the input is NULL.
 */
TreeNode *copyTree(const TreeNode *root);

/**
 * @brief Creates a new negation node ('~') for a given formula $\phi$.
 *
 * Creates $\sim\phi$ by assigning a deep copy of $\phi$ to the right child of the new '~' node.
 *
 * @param phi The root of the formula to be negated.
 * @return TreeNode* A pointer to the new negation tree node.
 */
TreeNode *negate_tree(TreeNode *phi);

/**
 * @brief Checks if a formula is a literal.
 *
 * A literal is an atom or a negated atom ($\sim$atom).
 *
 * @param phi A constant pointer to the root of the formula to check.
 * @return bool True if the formula is a literal, false otherwise.
 */
bool isLiteral(const TreeNode *phi);

/**
 * @brief Prints the propositional logic formula represented by the tree.
 *
 * Prints the formula using infix notation for binary operators and parentheses.
 *
 * @param f A constant pointer to the root of the formula tree.
 */
void print_formula(const TreeNode *f);

#endif
