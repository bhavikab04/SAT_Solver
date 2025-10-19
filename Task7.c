#include <stdio.h>
#include <string.h>
#include <stdbool.h>
#include <stdlib.h>
#include "Task7.h"
#include "Task2.h" // For TreeNode and isAtom definitions

// --- Data Structures for Literal Tracking ---

/**
 * @brief Represents the state of a literal (positive or negative).
 */
typedef enum {
    POSITIVE,
    NEGATIVE
} LiteralState;

/**
 * @brief Stores a literal's name and its state (negated or not).
 */
typedef struct {
    char* name;
    LiteralState state;
} LiteralRecord;

// --- Private Helper Function Prototypes ---

static void collectLiteralsInClause(TreeNode* node, LiteralRecord** list, int* count);
static bool isClauseTautology(TreeNode* clause_root);
static void checkCNFValidityRecursive(TreeNode* node, int* valid_clauses, int* invalid_clauses);


// --- Core Implementation ---

/**
 * @brief Recursively traverses a clause sub-tree to collect all literals.
 * A clause is a disjunction (ORs) of literals.
 */
static void collectLiteralsInClause(TreeNode* node, LiteralRecord** list, int* count) {
    if (node == NULL) {
        return;
    }

    // If the node is an OR ('+'), recurse on both children.
    if (strcmp(node->data, "+") == 0) {
        collectLiteralsInClause(node->left, list, count);
        collectLiteralsInClause(node->right, list, count);
    }
    // If the node is a NOT ('~'), it's a negative literal.
    else if (strcmp(node->data, "~") == 0) {
        (*count)++;
        *list = realloc(*list, (*count) * sizeof(LiteralRecord));
        (*list)[(*count) - 1].name = strdup(node->right->data);
        (*list)[(*count) - 1].state = NEGATIVE;
    }
    // Otherwise, it must be a positive literal (an atom).
    else if (isAtom(node->data)) {
        (*count)++;
        *list = realloc(*list, (*count) * sizeof(LiteralRecord));
        (*list)[(*count) - 1].name = strdup(node->data);
        (*list)[(*count) - 1].state = POSITIVE;
    }
}

/**
 * @brief Checks if a single clause is a tautology (e.g., contains p V ~p).
 * @param clause_root The root of the clause's sub-tree.
 * @return True if the clause is a tautology, false otherwise.
 */
static bool isClauseTautology(TreeNode* clause_root) {
    LiteralRecord* literals = NULL;
    int count = 0;
    collectLiteralsInClause(clause_root, &literals, &count);

    bool found_pair = false;
    // Compare every literal with every other literal.
    for (int i = 0; i < count; i++) {
        for (int j = i + 1; j < count; j++) {
            // Check if names are the same and states are opposite.
            if (strcmp(literals[i].name, literals[j].name) == 0 &&
                literals[i].state != literals[j].state) {
                found_pair = true;
                break;
            }
        }
        if (found_pair) break;
    }

    // Cleanup allocated memory for the list of literals.
    for (int i = 0; i < count; i++) {
        free(literals[i].name);
    }
    free(literals);

    return found_pair;
}


/**
 * @brief Recursively traverses the main CNF tree structure.
 * The CNF structure is a conjunction (ANDs) of clauses.
 */
static void checkCNFValidityRecursive(TreeNode* node, int* valid_clauses, int* invalid_clauses) {
    if (node == NULL) {
        return;
    }

    // If the node is an AND ('*'), the formula continues. Recurse on children.
    if (strcmp(node->data, "*") == 0) {
        checkCNFValidityRecursive(node->left, valid_clauses, invalid_clauses);
        checkCNFValidityRecursive(node->right, valid_clauses, invalid_clauses);
    }
    // If it's not an AND, it must be the root of a clause.
    else {
        if (isClauseTautology(node)) {
            (*valid_clauses)++;
        } else {
            (*invalid_clauses)++;
        }
    }
}


/**
 * @brief Checks if a given CNF formula tree is a tautology.
 * @param cnf_root The root of the CNF formula tree.
 * @param valid_clauses Pointer to store the count of valid clauses.
 * @param invalid_clauses Pointer to store the count of invalid clauses.
 * @return True if the formula is a tautology (all clauses are valid), false otherwise.
 */
bool checkCNFValidity(TreeNode* cnf_root, int* valid_clauses, int* invalid_clauses) {
    *valid_clauses = 0;
    *invalid_clauses = 0;

    if (cnf_root == NULL) {
        return true; // An empty formula is trivially a tautology.
    }

    checkCNFValidityRecursive(cnf_root, valid_clauses, invalid_clauses);

    // A CNF formula is a tautology if and only if every clause is a tautology.
    // This is equivalent to having zero invalid clauses.
    return (*invalid_clauses == 0);
}
