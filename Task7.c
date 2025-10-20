#include <stdio.h>
#include <string.h>
#include <stdbool.h>
#include <stdlib.h>
#include "Task7.h"
#include "Task2.h" // For TreeNode and isAtom definitions

#define HASH_TABLE_SIZE 256 // A reasonable size for literals within a single clause

// --- Data Structures for Efficient Literal Tracking ---

/**
 * @brief Stores which forms of a literal (positive, negative) have been seen.
 */
typedef struct LiteralPresence {
    char* name;
    bool has_positive;
    bool has_negative;
    struct LiteralPresence* next;
} LiteralPresence;

// --- Private Helper Function Prototypes ---
static bool isClauseTautology(TreeNode* clause_root);
static void checkCNFValidityRecursive(TreeNode* node, int* valid_clauses, int* invalid_clauses);
static void findLiteralsAndCheckTautology(TreeNode* node, LiteralPresence** hash_table, bool* is_tautology);

// --- Hash Table Helper Functions ---
static unsigned long hash(const char* str) {
    unsigned long hash = 5381;
    int c;
    while ((c = *str++))
        hash = ((hash << 5) + hash) + c; // djb2 algorithm
    return hash % HASH_TABLE_SIZE;
}

// --- Core Implementation (Optimized) ---

/**
 * @brief Recursively traverses a clause to find literals and check for tautology in a single pass.
 */
static void findLiteralsAndCheckTautology(TreeNode* node, LiteralPresence** hash_table, bool* is_tautology) {
    if (node == NULL || *is_tautology) {
        // Stop early if we've already found it's a tautology
        return;
    }

    if (strcmp(node->data, "+") == 0) {
        findLiteralsAndCheckTautology(node->left, hash_table, is_tautology);
        findLiteralsAndCheckTautology(node->right, hash_table, is_tautology);
    } else {
        // This node is a literal (or a negated literal)
        char* literal_name;
        bool is_negative = false;

        if (strcmp(node->data, "~") == 0 && node->right != NULL) {
            is_negative = true;
            literal_name = node->right->data;
        } else if (isAtom(node->data)) {
            literal_name = node->data;
        } else {
            return; // Should not happen in a valid clause
        }

        // Check the hash table
        unsigned long index = hash(literal_name);
        LiteralPresence* entry = hash_table[index];
        while (entry != NULL && strcmp(entry->name, literal_name) != 0) {
            entry = entry->next;
        }

        if (entry == NULL) {
            // First time seeing this literal, add it to the table
            entry = (LiteralPresence*)malloc(sizeof(LiteralPresence));
            entry->name = strdup(literal_name);
            entry->has_positive = !is_negative;
            entry->has_negative = is_negative;
            entry->next = hash_table[index];
            hash_table[index] = entry;
        } else {
            // Literal already seen, update its presence
            if (is_negative) entry->has_negative = true;
            else entry->has_positive = true;
        }

        // The magic: check if both forms are now present
        if (entry->has_positive && entry->has_negative) {
            *is_tautology = true;
        }
    }
}

/**
 * @brief (OPTIMIZED) Checks if a single clause is a tautology using a hash table.
 * Runtime is O(L) where L is the number of literals in the clause.
 */
static bool isClauseTautology(TreeNode* clause_root) {
    if (clause_root == NULL) return false;

    bool is_tautology = false;
    // Allocate and initialize the hash table
    LiteralPresence** hash_table = (LiteralPresence**)calloc(HASH_TABLE_SIZE, sizeof(LiteralPresence*));

    findLiteralsAndCheckTautology(clause_root, hash_table, &is_tautology);

    // Cleanup: Free the hash table and its contents
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
 * @brief Recursively traverses the main CNF structure (ANDs of clauses).
 */
static void checkCNFValidityRecursive(TreeNode* node, int* valid_clauses, int* invalid_clauses) {
    if (node == NULL) {
        return;
    }

    if (strcmp(node->data, "*") == 0) {
        checkCNFValidityRecursive(node->left, valid_clauses, invalid_clauses);
        checkCNFValidityRecursive(node->right, valid_clauses, invalid_clauses);
    } else {
        // Assumed to be the root of a clause
        if (isClauseTautology(node)) {
            (*valid_clauses)++;
        } else {
            (*invalid_clauses)++;
        }
    }
}

/**
 * @brief Public entry point to check CNF validity.
 */
bool checkCNFValidity(TreeNode* cnf_root, int* valid_clauses, int* invalid_clauses) {
    *valid_clauses = 0;
    *invalid_clauses = 0;

    if (cnf_root == NULL) {
        return true; // An empty formula is trivially valid.
    }

    checkCNFValidityRecursive(cnf_root, valid_clauses, invalid_clauses);

    // A CNF formula is a tautology if and only if ALL of its clauses are.
    // Therefore, if there are any invalid clauses, the whole formula is not a tautology.
    return (*invalid_clauses == 0);
}
