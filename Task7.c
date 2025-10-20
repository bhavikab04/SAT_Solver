#include <stdio.h>
#include <string.h>
#include <stdbool.h>
#include <stdlib.h>
#include "Task7.h"
#include "Task2.h"

/**
 * @file Task7.c
 * @brief Implements the checker for CNF tautologies.
 *
 * This file contains the logic for determining if a formula in Conjunctive
 * Normal Form is always true. It works by checking each clause for the
 * presence of a literal and its opposite (e.g., `p` and `~p`).
 */

#define HASH_TABLE_SIZE 256 ///< Defines the hash table size for checking a single clause.

/**
 * @struct LiteralPresence
 * @brief A simple structure to track if a literal's positive and negative forms appear.
 *
 * This is used in a temporary hash table to quickly determine if a clause
 * contains a variable and its negation.
 */
typedef struct LiteralPresence {
    char* name;
    bool has_positive;
    bool has_negative;
    struct LiteralPresence* next;
} LiteralPresence;

// --- Internal Helper Function Prototypes ---
static bool isClauseTautology(TreeNode* clause_root);
static void findLiteralsInClause(TreeNode* node, LiteralPresence** hash_table, bool* is_tautology);
static void checkCNFValidityRecursive(TreeNode* node, int* valid_clauses, int* invalid_clauses);
static unsigned long hash(const char* str);

/**
 * @brief Generates a hash value from a string.
 * @param str The string to be hashed.
 * @return A numerical hash index.
 */
static unsigned long hash(const char* str) {
    unsigned long hash = 5381;
    int c;
    while ((c = *str++))
        hash = ((hash << 5) + hash) + c;
    return hash % HASH_TABLE_SIZE;
}

/**
 * @brief Traverses a clause to find literals and check for a tautology.
 *
 * This function navigates the subtree of a clause. It uses a temporary hash
 * table to log every literal it sees. If it finds that both the positive (p) and
 * negative (~p) forms of a literal are present, it marks the clause as a
 * tautology and stops checking.
 *
 * @param node The current node in the clause subtree.
 * @param hash_table A temporary hash table for tracking literals in this clause.
 * @param[in,out] is_tautology A pointer to a flag that is set to true if a tautology is found.
 */
static void findLiteralsInClause(TreeNode* node, LiteralPresence** hash_table, bool* is_tautology) {
    if (node == NULL || *is_tautology) return;

    if (strcmp(node->data, "+") == 0) {
        findLiteralsInClause(node->left, hash_table, is_tautology);
        findLiteralsInClause(node->right, hash_table, is_tautology);
    } else {
        char* literal_name;
        bool is_negative = false;

        if (strcmp(node->data, "~") == 0 && node->right != NULL) {
            is_negative = true;
            literal_name = node->right->data;
        } else if (isAtom(node->data)) {
            literal_name = node->data;
        } else {
            return;
        }

        unsigned long index = hash(literal_name);
        LiteralPresence* entry = hash_table[index];
        while (entry != NULL && strcmp(entry->name, literal_name) != 0) {
            entry = entry->next;
        }

        if (entry == NULL) {
            entry = (LiteralPresence*)malloc(sizeof(LiteralPresence));
            entry->name = strdup(literal_name);
            entry->has_positive = !is_negative;
            entry->has_negative = is_negative;
            entry->next = hash_table[index];
            hash_table[index] = entry;
        } else {
            if (is_negative) entry->has_negative = true;
            else entry->has_positive = true;
        }

        if (entry->has_positive && entry->has_negative) {
            *is_tautology = true;
        }
    }
}

/**
 * @brief Checks if an individual clause is a tautology.
 *
 * A clause is a tautology if it contains a literal and its negation (L and ~L).
 * This function sets up a hash table and calls a recursive helper to efficiently
 * check for this condition.
 *
 * @param clause_root The root node of the clause's subtree.
 * @return Returns true if the clause is a tautology, and false otherwise.
 */
static bool isClauseTautology(TreeNode* clause_root) {
    if (clause_root == NULL) return false;

    bool is_tautology = false;
    LiteralPresence** hash_table = (LiteralPresence**)calloc(HASH_TABLE_SIZE, sizeof(LiteralPresence*));

    findLiteralsInClause(clause_root, hash_table, &is_tautology);

    for (int i = 0; i < HASH_TABLE_SIZE; i++) {
        LiteralPresence* entry = hash_table[i];
        while (entry != NULL) {
            LiteralPresence* temp = entry;
            entry = entry->next;
            free(temp->name);
            free(temp);
        }
    }
    free(hash_table);

    return is_tautology;
}

/**
 * @brief Recursively navigates the CNF structure to check each clause.
 *
 * This function moves down the tree of conjunctions ('*'). When it finds a
 * node that is not a conjunction, it assumes that node is the root of a clause
 * and sends it to be validated.
 *
 * @param node The current node in the CNF tree.
 * @param[out] valid_clauses A counter for valid clauses.
 * @param[out] invalid_clauses A counter for invalid clauses.
 */
static void checkCNFValidityRecursive(TreeNode* node, int* valid_clauses, int* invalid_clauses) {
    if (node == NULL) return;

    if (strcmp(node->data, "*") == 0) {
        checkCNFValidityRecursive(node->left, valid_clauses, invalid_clauses);
        checkCNFValidityRecursive(node->right, valid_clauses, invalid_clauses);
    } else {
        if (isClauseTautology(node)) {
            (*valid_clauses)++;
        } else {
            (*invalid_clauses)++;
        }
    }
}

bool checkCNFValidity(TreeNode* cnf_root, int* valid_clauses, int* invalid_clauses) {
    *valid_clauses = 0;
    *invalid_clauses = 0;

    if (cnf_root == NULL) {
        return true;
    }

    checkCNFValidityRecursive(cnf_root, valid_clauses, invalid_clauses);

    return (*invalid_clauses == 0);
}