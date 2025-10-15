#ifndef TASK6_H
#define TASK6_H

#include <stdio.h>
#include <stdlib.h>
#include <string.h>

// Type definitions:
typedef enum
{
    ATOM,
    NOT,
    AND,
    OR,
    IMP
} FormulaType;

typedef struct Formula
{
    FormulaType type;
    char *symbol; // For ATOMs only (e.g., "p", "q")
    struct Formula *left;
    struct Formula *right;
} Formula;

// Core Algorithms:
// Implication free algorithm:
Formula *IMPL_FREE(Formula *phi);

// Negation normal form function:
Formula *NNF(Formula *phi);

// Distibutive function:
Formula *DISTR(Formula *eta1, Formula *eta2);

// Main function to convert NNF to CNF and recursively applies distributuion function
Formula *CNF(Formula *phi);

// Driver function that executes 3 steps: IMPL_FREE, NNF and CNF
Formula *CNF_FORMULA(Formula *phi);

// Helper functions:
// Create formula node by allocating memory and initializing fields:
Formula *create_formula(FormulaType type, const char *sym, Formula *lhs, Formula *rhs);

// Free formula using post-order traversal:
void free_formula(Formula *f);

// Function to copy formula:
Formula *copy_formula(const Formula *f);

// Function to negate formula:
Formula *negate_formula(Formula *phi);

// Function to check if it's a literal:
int is_literal(const Formula *phi);

// Function to print the formula:
void print_formula(const Formula *f);

#endif
