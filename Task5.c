#include <stdio.h>
#include <stdlib.h>
#include <stdbool.h>
#include <string.h>
#include "Task5.h"
#include "Task2.h"

/**
 * @file Task5.c
 * @brief Implementation of the formula evaluation and truth table logic.
 *
 * This file provides the core functions for working with logical formulas.
 * An efficient hash table is used to manage variable assignments, which is
 * key to the performance of the evaluation functions.
 */

#define HASH_TABLE_SIZE 2048 ///< Defines the number of buckets in the hash table.

// --- Internal Helper Function Prototypes ---
static void collectLiteralsRecursive(TreeNode* node, AssignmentHashTable* ht);
static void generateAssignmentsAndPrint(TreeNode* root, char** literals, int count, int index, AssignmentHashTable* current_assignments);
static unsigned long hash(const char *str);

/**
 * @struct AssignmentNode
 * @brief Represents a single variable-to-value mapping in the hash table.
 *
 * Each node stores the variable's name, its boolean value, and a pointer
 * to the next node to handle potential hash collisions.
 */
typedef struct AssignmentNode {
    char* literal;
    bool value;
    struct AssignmentNode* next;
} AssignmentNode;

/**
 * @struct AssignmentHashTable
 * @brief The internal structure of the hash table for variable assignments.
 *
 * It consists of an array of pointers, where each pointer acts as the head
 * of a linked list for storing AssignmentNode objects.
 */
struct AssignmentHashTable {
    AssignmentNode* table[HASH_TABLE_SIZE];
};

/**
 * @brief Generates a hash value from a string.
 * @param str The string (variable name) to be hashed.
 * @return A numerical hash value to be used as an array index.
 */
static unsigned long hash(const char *str) {
    unsigned long hash = 5381;
    int c;
    while ((c = *str++))
        hash = ((hash << 5) + hash) + c;
    return hash % HASH_TABLE_SIZE;
}

/**
 * @brief Allocates memory for a new, empty hash table.
 * @return A pointer to the newly created hash table.
 */
static AssignmentHashTable* createAssignmentHashTable() {
    return (AssignmentHashTable*)calloc(1, sizeof(AssignmentHashTable));
}

/**
 * @brief Frees all memory used by the hash table.
 * @param ht A pointer to the hash table that needs to be deallocated.
 */
static void freeAssignmentHashTable(AssignmentHashTable* ht) {
    if (!ht) return;
    for (int i = 0; i < HASH_TABLE_SIZE; i++) {
        AssignmentNode* current = ht->table[i];
        while (current) {
            AssignmentNode* temp = current;
            current = current->next;
            free(temp->literal);
            free(temp);
        }
    }
    free(ht);
}

/**
 * @brief Inserts a new variable or updates an existing one in the hash table.
 * @note This function checks if the variable already exists. If it does, the
 * function just updates the value. Otherwise, it creates a new entry. This
 * "update-or-insert" behavior is critical for the efficiency of generating
 * a truth table.
 *
 * @param ht A pointer to the hash table.
 * @param literal The name of the variable to add or update.
 * @param value The boolean value to assign to the variable.
 */
static void assignmentHT_Insert(AssignmentHashTable* ht, const char* literal, bool value) {
    unsigned long index = hash(literal);
    AssignmentNode* current = ht->table[index];

    while (current) {
        if (strcmp(current->literal, literal) == 0) {
            current->value = value;
            return;
        }
        current = current->next;
    }

    AssignmentNode* newNode = (AssignmentNode*)malloc(sizeof(AssignmentNode));
    newNode->literal = strdup(literal);
    newNode->value = value;
    newNode->next = ht->table[index];
    ht->table[index] = newNode;
}

/**
 * @brief Looks up a variable's value in the hash table.
 * @note This lookup is very fast, with an average time complexity of O(1).
 *
 * @param ht A pointer to the hash table to search.
 * @param literal The name of the variable to find.
 * @param[out] out_value A pointer to a boolean where the found value is stored.
 * @return Returns true if the variable was found, and false otherwise.
 */
static bool assignmentHT_Get(const AssignmentHashTable* ht, const char* literal, bool* out_value) {
    unsigned long index = hash(literal);
    AssignmentNode* current = ht->table[index];
    while (current) {
        if (strcmp(current->literal, literal) == 0) {
            *out_value = current->value;
            return true;
        }
        current = current->next;
    }
    return false;
}

// --- Public Function Implementations ---

bool evaluateTree(TreeNode *root, const AssignmentHashTable *assignments) {
    if (!root || !root->data) return false;

    if (isAtom(root->data)) {
        bool value;
        if (assignmentHT_Get(assignments, root->data, &value)) {
            return value;
        }
        return false; // Assume false if a variable is not assigned.
    }

    char op = root->data[0];
    bool rightVal = evaluateTree(root->right, assignments);
    if (op == '~') return !rightVal;

    bool leftVal = evaluateTree(root->left, assignments);
    switch (op) {
        case '+': return leftVal || rightVal;
        case '*': return leftVal && rightVal;
        case '>': return !leftVal || rightVal;
        default: return false;
    }
}

bool evaluateFromFile(TreeNode* root, const char* filename) {
    FILE *fp = fopen(filename, "r");
    if (!fp) return false;

    AssignmentHashTable* assignments_ht = createAssignmentHashTable();
    char line_buffer[256];
    while (fgets(line_buffer, sizeof(line_buffer), fp)) {
        char literal_buffer[128];
        char value_char;
        if (sscanf(line_buffer, "%127s = %c", literal_buffer, &value_char) == 2) {
            assignmentHT_Insert(assignments_ht, literal_buffer, (value_char == 'T' || value_char == 't'));
        }
    }
    fclose(fp);

    evaluateTree(root, assignments_ht);
    freeAssignmentHashTable(assignments_ht);
    return true;
}

/**
 * @brief Helper function to recursively find all unique literals in the tree.
 * @param node The current node being visited in the tree.
 * @param ht The hash table used to keep track of literals already found.
 */
static void collectLiteralsRecursive(TreeNode* node, AssignmentHashTable* ht) {
    if (node == NULL) return;
    if (isAtom(node->data)) {
        assignmentHT_Insert(ht, node->data, false);
    }
    collectLiteralsRecursive(node->left, ht);
    collectLiteralsRecursive(node->right, ht);
}

int collectUniqueLiterals(TreeNode *root, char ***literals_list_out) {
    AssignmentHashTable* ht = createAssignmentHashTable();
    collectLiteralsRecursive(root, ht);

    int count = 0;
    for (int i = 0; i < HASH_TABLE_SIZE; i++) {
        for (AssignmentNode* node = ht->table[i]; node != NULL; node = node->next) count++;
    }

    if (count > 0) {
        *literals_list_out = (char**)malloc(count * sizeof(char*));
        int k = 0;
        for (int i = 0; i < HASH_TABLE_SIZE; i++) {
            for (AssignmentNode* node = ht->table[i]; node != NULL; node = node->next) {
                (*literals_list_out)[k++] = strdup(node->literal);
            }
        }
    } else {
        *literals_list_out = NULL;
    }

    freeAssignmentHashTable(ht);
    return count;
}

void printTruthTable(TreeNode* root, char** literals, int count, const char* formula_str) {
    if (count == 0 || !root) return;
    if (count > MAX_TRUTH_TABLE_VARIABLES) {
        printf("Skipping truth table: %d variables exceeds limit of %d.\n", count, MAX_TRUTH_TABLE_VARIABLES);
        return;
    }

    for (int i = 0; i < count; i++) printf("| %-5s ", literals[i]);
    printf("| %s |\n", formula_str);
    
    AssignmentHashTable* current_assignments = createAssignmentHashTable();
    for (int i = 0; i < count; i++) {
        assignmentHT_Insert(current_assignments, literals[i], false);
    }

    generateAssignmentsAndPrint(root, literals, count, 0, current_assignments);
    freeAssignmentHashTable(current_assignments);
}

/**
 * @brief Recursively generates and prints each row of the truth table.
 *
 * This function operates by assigning `false` to a variable, then calling
 * itself to handle the remaining variables. After that returns, it assigns
 * `true` to the same variable and calls itself again. This process builds every
 * possible combination. A row is printed when all variables have been assigned.
 *
 * @param root The root of the formula tree.
 * @param literals An array of all variable names.
 * @param count The total number of variables.
 * @param index The current variable being processed.
 * @param current_assignments The hash table storing assignments for the current row.
 */
static void generateAssignmentsAndPrint(TreeNode* root, char** literals, int count, int index, AssignmentHashTable* current_assignments) {
    if (index == count) {
        for (int i = 0; i < count; i++) {
            bool value;
            assignmentHT_Get(current_assignments, literals[i], &value);
            printf("| %-5s ", value ? "T" : "F");
        }
        bool result = evaluateTree(root, current_assignments);
        printf("| %-5s |\n", result ? "True" : "False");
        return;
    }

    assignmentHT_Insert(current_assignments, literals[index], false);
    generateAssignmentsAndPrint(root, literals, count, index + 1, current_assignments);

    assignmentHT_Insert(current_assignments, literals[index], true);
    generateAssignmentsAndPrint(root, literals, count, index + 1, current_assignments);
}