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


static void Track_Unique_Literals(TreeNode* node, Assign_HT* ht);
static void Assign_value(TreeNode* root, char** literals, int count, int index, Assign_HT* current_assignments);
static unsigned long hash(const char *str);

/**
 * @struct Assign
 * @brief Represents a single variable-to-value mapping in the hash table.
 *
 * Each node stores the variable's name, its boolean value, and a pointer
 * to the next node to handle potential hash collisions.
 */
typedef struct Assign {
    char* literal;
    bool value;
    struct Assign* next;
} Assign;

/**
 * @struct Assign_HT
 * @brief The internal structure of the hash table for variable assignments.
 *
 * It consists of an array of pointers, where each pointer acts as the head
 * of a linked list for storing Assign objects.
 */
struct Assign_HT {
    Assign* table[HASH_TABLE_SIZE];
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
static Assign_HT* createAssign_HT() {
    return (Assign_HT*)calloc(1, sizeof(Assign_HT));
}

/**
 * @brief Frees all memory used by the hash table.
 * @param ht A pointer to the hash table that needs to be deallocated.
 */
static void freeAssign_HT(Assign_HT* ht) {
    if (!ht) return;
    for (int i = 0; i < HASH_TABLE_SIZE; i++) {
        Assign* current = ht->table[i];
        while (current) {
            Assign* temp = current;
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
static void HT_Insert(Assign_HT* ht, const char* literal, bool value) {
    unsigned long index = hash(literal);
    Assign* current = ht->table[index];

    while (current) {
        if (strcmp(current->literal, literal) == 0) {
            current->value = value;
            return;
        }
        current = current->next;
    }

    Assign* newNode = (Assign*)malloc(sizeof(Assign));
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
static bool HT_Get(const Assign_HT* ht, const char* literal, bool* out_value) {
    unsigned long index = hash(literal);
    Assign* current = ht->table[index];
    while (current) {
        if (strcmp(current->literal, literal) == 0) {
            *out_value = current->value;
            return true;
        }
        current = current->next;
    }
    return false;
}


bool evaluateTree(TreeNode *root, const Assign_HT *assignments) {
    if (!root || !root->data) return false;

    if (isAtom(root->data)) {
        bool value;
        if (HT_Get(assignments, root->data, &value)) {
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

    Assign_HT* assignments_ht = createAssign_HT();
    char line_buffer[256];
    while (fgets(line_buffer, sizeof(line_buffer), fp)) {
        char literal_buffer[128];
        char value_char;
        if (sscanf(line_buffer, "%127s = %c", literal_buffer, &value_char) == 2) {
            HT_Insert(assignments_ht, literal_buffer, (value_char == 'T' || value_char == 't'));
        }
    }
    fclose(fp);

    evaluateTree(root, assignments_ht);
    freeAssign_HT(assignments_ht);
    return true;
}

/**
 * @brief Helper function to recursively find all unique literals in the tree.
 * @param node The current node being visited in the tree.
 * @param ht The hash table used to keep track of literals already found.
 */
static void Track_Unique_Literals(TreeNode* node, Assign_HT* ht) {
    if (node == NULL) return;
    if (isAtom(node->data)) {
        HT_Insert(ht, node->data, false);
    }
    Track_Unique_Literals(node->left, ht);
    Track_Unique_Literals(node->right, ht);
}

int get_Unique_Literals(TreeNode *root, char ***literals_list_out) {
    Assign_HT* ht = createAssign_HT();
    Track_Unique_Literals(root, ht);

    int count = 0;
    for (int i = 0; i < HASH_TABLE_SIZE; i++) {
        for (Assign* node = ht->table[i]; node != NULL; node = node->next) count++;
    }

    if (count > 0) {
        *literals_list_out = (char**)malloc(count * sizeof(char*));
        int k = 0;
        for (int i = 0; i < HASH_TABLE_SIZE; i++) {
            for (Assign* node = ht->table[i]; node != NULL; node = node->next) {
                (*literals_list_out)[k++] = strdup(node->literal);
            }
        }
    } else {
        *literals_list_out = NULL;
    }

    freeAssign_HT(ht);
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
    
    Assign_HT* current_assignments = createAssign_HT();
    for (int i = 0; i < count; i++) {
        HT_Insert(current_assignments, literals[i], false);
    }

    Assign_value(root, literals, count, 0, current_assignments);
    freeAssign_HT(current_assignments);
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
static void Assign_value(TreeNode* root, char** literals, int count, int index, Assign_HT* current_assignments) {
    if (index == count) {
        for (int i = 0; i < count; i++) {
            bool value;
            HT_Get(current_assignments, literals[i], &value);
            printf("| %-5s ", value ? "T" : "F");
        }
        bool result = evaluateTree(root, current_assignments);
        printf("| %-5s |\n", result ? "True" : "False");
        return;
    }

    HT_Insert(current_assignments, literals[index], false);
    Assign_value(root, literals, count, index + 1, current_assignments);

    HT_Insert(current_assignments, literals[index], true);
    Assign_value(root, literals, count, index + 1, current_assignments);
}