#include <stdio.h>
#include <stdlib.h>
#include <stdbool.h>
#include <string.h>
#include "Task5.h"
#include "Task2.h" // For TreeNode and isAtom definitions

/**
 * @file Task5.c
 * @brief Implements formula evaluation and truth table generation.
 *
 * This file provides functions to evaluate logical formulas using an
 * efficient hash table for variable assignments.
 */

#define HASH_TABLE_SIZE 2048 // Defines the number of buckets in the hash table.

// Global variables for limiting truth table output rows.
static int g_line_counter = 0;
static bool g_limit_output = false;

static void Track_Unique_Literals(TreeNode* node, Assign_HT* ht);
static void Assign_value(TreeNode* root, char** literals, int count, int index, Assign_HT* current_assignments);
static unsigned long hash(const char *str);

/**
 * @struct Assign
 * @brief Represents a single variable assignment in a hash table bucket.
 */
typedef struct Assign {
    char* literal;  // The variable name.
    bool value;     // The assigned boolean value.
    struct Assign* next; ///< Pointer to the next node for collision handling.
} Assign;

/**
 * @struct Assign_HT
 * @brief The internal structure of the hash table.
 *
 * An array of pointers, where each pointer is the head of a linked list.
 */
struct Assign_HT {
    Assign* table[HASH_TABLE_SIZE];
};

/**
 * @brief Generates a hash index from a string (djb2 algorithm).
 * @param str The string to hash.
 * @return A numerical hash value for an array index.
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
 * @brief Frees all memory used by the hash table and its nodes.
 * @param ht A pointer to the hash table to deallocate.
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
 * @brief Inserts or updates a variable's value in the hash table.
 *
 * If the literal exists, its value is updated. Otherwise, a new
 * entry is created. This is efficient for recursive calls.
 *
 * @param ht A pointer to the hash table.
 * @param literal The name of the variable.
 * @param value The boolean value to assign.
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
 * @brief Retrieves a variable's value from the hash table.
 *
 * @param ht A pointer to the hash table to search.
 * @param literal The name of the variable to find.
 * @param[out] out_value A pointer to store the found boolean value.
 * @return Returns true if the variable was found, false otherwise.
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
        return false; // Default to false if not assigned.
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
 * @brief Helper function to recursively find all unique literals.
 * @param node The current node being visited.
 * @param ht The hash table for tracking found literals.
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

/**
 * @brief Prints a truth table, limiting output for large variable counts.
 *
 * If the variable count exceeds MAX_TRUTH_TABLE_VARIABLES, it prints a
 * warning and stops after 100 rows.
 *
 * @param root The root of the formula tree.
 * @param literals An array of all unique variable names.
 * @param count The total number of unique variables.
 * @param formula_str The formula as a string for the header.
 */
void printTruthTable(TreeNode *root, char **literals, int count, const char *formula_str) {
    if (count == 0 || !root) return;

    if (count > MAX_TRUTH_TABLE_VARIABLES) {
        printf("Warning: %d variables exceeds limit of %d.\n", count, MAX_TRUTH_TABLE_VARIABLES);
        printf("Printing only the first 100 lines of the truth table.\n");
        g_limit_output = true;
    } else {
        g_limit_output = false;
    }
    
    // Print table header
    for (int i = 0; i < count; i++) printf("| %-5s ", literals[i]);
    printf("| %s |\n", formula_str);
    
    Assign_HT* current_assignments = createAssign_HT();
    for (int i = 0; i < count; i++) {
        HT_Insert(current_assignments, literals[i], false);
    }

    g_line_counter = 0; // Reset line counter
    
    Assign_value(root, literals, count, 0, current_assignments);
    
    freeAssign_HT(current_assignments);
}

/**
 * @brief Recursively generates and prints rows for the truth table.
 *
 * This function assigns true/false to each variable. It checks the global
 * line counter to stop printing if the 100-line limit is reached.
 *
 * @param root The root of the formula tree.
 * @param literals An array of all variable names.
 * @param count The total number of variables.
 * @param index The index of the current variable being processed.
 * @param current_assignments The hash table holding the current assignments.
 */
static void Assign_value(TreeNode* root, char** literals, int count, int index, Assign_HT* current_assignments) {
    if (g_limit_output && g_line_counter >= 100) {
        return;
    }
    
    if (index == count) {
        g_line_counter++; // Count this printed line
        
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

    if (g_limit_output && g_line_counter >= 100) {
        return;
    }

    HT_Insert(current_assignments, literals[index], true);
    Assign_value(root, literals, count, index + 1, current_assignments);
}