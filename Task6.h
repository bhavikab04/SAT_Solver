#ifndef TASK6_H
#define TASK6_H

#include <stdio.h>
#include <stdlib.h>
#include <string.h>

#include "Task2.h" //need it for struct and isAtom() function
#include "Task3.h" //need it for freeTree() function

// Core Algorithms:
// Implication free algorithm:
TreeNode *IMPL_FREE(TreeNode *phi);

// Negation normal form function:
TreeNode *NNF(TreeNode *phi);

// Distibutive function:
TreeNode *DISTR(TreeNode *eta1, TreeNode *eta2);

// Main function to convert NNF to CNF and recursively applies distributuion function
TreeNode *CNF(TreeNode *phi);

// Driver function that executes 3 steps: IMPL_FREE, NNF and CNF
TreeNode *CNF_FORMULA(TreeNode *phi);

// Function to take a deep copy of the TreeNode structure:
TreeNode *copyTree(const TreeNode *root);

// Function to negate formula:
TreeNode *negate_tree(TreeNode *phi);

// Function to check if it's a literal (atom or negated atom):
bool isLiteral(const TreeNode *phi);

// Function to print the output by recursive in order traversal
void print_formula(const TreeNode *f);

#endif
