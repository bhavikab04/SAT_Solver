#include <stdio.h>
#include <stdlib.h>
#include <stdbool.h>
#include <string.h>
#include "Task5.h"
#include "Task2.h" // For TreeNode, isAtom definitions

#define HASH_TABLE_SIZE 1024

// --- Data Structures for High-Efficiency Literal Collection (Hash Table) ---
typedef struct HashNode {
    char* key;
    struct HashNode* next;
} HashNode;

typedef struct {
    HashNode* table[HASH_TABLE_SIZE];
} HashTable;

// --- Private Helper Prototypes ---
static void collectLiteralsRecursive(TreeNode* node, HashTable* ht);

// --- Hash Table Helper Functions (Efficiently find unique literals) ---
static unsigned long hash(const char *str) {
    unsigned long hash = 5381;
    int c;
    while ((c = *str++))
        hash = ((hash << 5) + hash) + c;
    return hash % HASH_TABLE_SIZE;
}

static HashTable* createHashTable() {
    return (HashTable*)calloc(1, sizeof(HashTable));
}

static void hashTableInsert(HashTable* ht, const char* key) {
    unsigned long index = hash(key);
    HashNode* current = ht->table[index];
    while (current) {
        if (strcmp(current->key, key) == 0) return; // Key already exists
        current = current->next;
    }
    HashNode* newNode = (HashNode*)malloc(sizeof(HashNode));
    newNode->key = strdup(key);
    newNode->next = ht->table[index];
    ht->table[index] = newNode;
}

static void freeHashTable(HashTable* ht) {
    if (!ht) return;
    for (int i = 0; i < HASH_TABLE_SIZE; i++) {
        HashNode* current = ht->table[i];
        while (current) {
            HashNode* temp = current;
            current = current->next;
            free(temp->key);
            free(temp);
        }
    }
    free(ht);
}


// --- Core Task 5 Functions ---

/**
 * @brief Evaluates the truth value of the expression tree for a given set of literal assignments.
 */
bool evaluateTree(TreeNode *root, const TruthAssignment assignments[], int num_assignments) {
    if (root == NULL || root->data == NULL) {
        // Should not happen in a valid tree, but good to handle.
        return false;
    }

    // Check if the node's data is a literal (atom)
    if (isAtom(root->data)) {
        for (int i = 0; i < num_assignments; i++) {
            // Use strcmp to find the matching literal in the assignments array
            if (strcmp(root->data, assignments[i].literal) == 0) {
                return assignments[i].value;
            }
        }
        // If a literal is not found in the assignments, it's an error.
        // Defaulting to false.
        fprintf(stderr, "Warning: No assignment found for literal '%s'. Defaulting to false.\n", root->data);
        return false;
    }

    // Handle operators. Since they are single chars, we can check the first character.
    char op = root->data[0];

    // The right child is always evaluated for both unary and binary operators.
    bool rightVal = evaluateTree(root->right, assignments, num_assignments);

    if (op == '~') {
        return !rightVal;
    }

    // For binary operators, we also need the left child's value.
    bool leftVal = evaluateTree(root->left, assignments, num_assignments);

    switch (op) {
        case '+': // OR
            return leftVal || rightVal;
        case '*': // AND
            return leftVal && rightVal;
        case '>': // IMPLIES (equivalent to !left OR right)
            return !leftVal || rightVal;
        default:
            // Should not be reached with a validly constructed tree.
            return false;
    }
}

/**
 * @brief Recursively traverses the tree to find all unique literals.
 */
static void collectLiteralsRecursive(TreeNode* node, HashTable* ht) {
    if (node == NULL) {
        return;
    }
    // isAtom is defined in Task2 and correctly identifies non-operators
    if (isAtom(node->data)) {
        hashTableInsert(ht, node->data);
    }
    collectLiteralsRecursive(node->left, ht);
    collectLiteralsRecursive(node->right, ht);
}

/**
 * @brief Public function to collect all unique literals from the tree.
 * Returns the count and populates an array of strings.
 */
int collectUniqueLiterals(TreeNode *root, char ***literals_list_out) {
    HashTable* ht = createHashTable();
    collectLiteralsRecursive(root, ht);

    int count = 0;
    for (int i = 0; i < HASH_TABLE_SIZE; i++) {
        for (HashNode* node = ht->table[i]; node != NULL; node = node->next) {
            count++;
        }
    }

    if (count > 0) {
        *literals_list_out = (char**)malloc(count * sizeof(char*));
        int k = 0;
        for (int i = 0; i < HASH_TABLE_SIZE; i++) {
            for (HashNode* node = ht->table[i]; node != NULL; node = node->next) {
                (*literals_list_out)[k++] = strdup(node->key);
            }
        }
    } else {
        *literals_list_out = NULL;
    }

    freeHashTable(ht);
    return count;
}


/**
 * @brief Recursive helper to generate all possible truth assignments and print table rows.
 */
static void generateAssignmentsAndPrint(TreeNode* root, char** literals, int count, int index, TruthAssignment* current_assignments) {
    // Base case: a full assignment is ready, evaluate and print the row.
    if (index == count) {
        for (int i = 0; i < count; i++) {
            printf("| %-5s ", current_assignments[i].value ? "T" : "F");
        }
        bool result = evaluateTree(root, current_assignments, count);
        printf("| %-5s |\n", result ? "T" : "F");
        return;
    }

    // Recursive step: branch for the current literal.
    // Branch 1: Set current literal to FALSE
    current_assignments[index].literal = literals[index];
    current_assignments[index].value = false;
    generateAssignmentsAndPrint(root, literals, count, index + 1, current_assignments);

    // Branch 2: Set current literal to TRUE
    current_assignments[index].literal = literals[index];
    current_assignments[index].value = true;
    generateAssignmentsAndPrint(root, literals, count, index + 1, current_assignments);
}


/**
 * @brief Generates and prints the complete truth table for the formula.
 */
void printTruthTable(TreeNode* root, char** literals, int count, const char* formula_str) {
    if (count == 0 || root == NULL) {
        return;
    }

    // 1. Print header
    int total_width = 0;
    for (int i = 0; i < count; i++) {
        printf("| %-5s ", literals[i]);
        total_width += 8;
    }
    printf("| %-5s |\n", formula_str);
    total_width += 8;

    for (int i = 0; i < total_width; i++) printf("-");
    printf("\n");

    // 2. Start recursive generation of assignments
    TruthAssignment* current_assignments = (TruthAssignment*)malloc(count * sizeof(TruthAssignment));
    generateAssignmentsAndPrint(root, literals, count, 0, current_assignments);
    free(current_assignments);
}
