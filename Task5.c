#include <stdio.h>
#include <stdlib.h>
#include <stdbool.h>
#include <string.h>
#include <ctype.h>
#include "Task5.h"
#include "Task2.h" // For TreeNode structure

#define HASH_TABLE_SIZE 1024 // A reasonable size for a hash table, can be adjusted.

// --- Data Structures for High-Efficiency Literal Collection (Hash Table) ---
typedef struct HashNode {
    char* key;
    struct HashNode* next;
} HashNode;

typedef struct {
    HashNode* table[HASH_TABLE_SIZE];
} HashTable;

// --- Helper Functions for the Hash Table (Kept as provided) ---

// djb2 hash function: simple and effective for strings.
static unsigned long hash(const char *str) {
    unsigned long hash = 5381;
    int c;
    while ((c = *str++))
        hash = ((hash << 5) + hash) + c; /* hash * 33 + c */
    return hash % HASH_TABLE_SIZE;
}

// Creates and initializes a new hash table.
static HashTable* createHashTable() {
    HashTable* ht = calloc(1, sizeof(HashTable));
    return ht;
}

// Inserts a new literal into the hash table if it doesn't already exist.
static void hashTableInsert(HashTable* ht, const char* key) {
    unsigned long index = hash(key);
    HashNode* current = ht->table[index];
    // Check if the key already exists in the chain
    while (current) {
        if (strcmp(current->key, key) == 0) {
            return; // Key already exists, do nothing.
        }
        current = current->next;
    }
    // Key doesn't exist, insert at the beginning of the list (chaining)
    HashNode* newNode = malloc(sizeof(HashNode));
    newNode->key = strdup(key);
    newNode->next = ht->table[index];
    ht->table[index] = newNode;
}

// Frees all memory used by the hash table.
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

// --- CORE LOGIC (Kept as provided) ---

/**
 * NOTE: This function is MODIFIED to support string-based atoms (`char*`).
 * The logic remains the same, but it now uses `strcmp` for comparison.
 */
bool evaluateTree(TreeNode *root, const TruthAssignment assignments[], int num_assignments) {
    if (root == NULL) {
        fprintf(stderr, "Error: Attempted to evaluate a NULL node.\n");
        exit(EXIT_FAILURE);
    }

    // A leaf node is an atom/literal.
    if (root->left == NULL && root->right == NULL) {
        for (int i = 0; i < num_assignments; i++) {
            // Use string comparison instead of char comparison
            if (strcmp(assignments[i].literal, root->data) == 0) {
                return assignments[i].value;
            }
        }
        // This case should not happen if assignments are generated correctly for the formula
        fprintf(stderr, "Error: Truth value for literal '%s' not found.\n", root->data);
        exit(EXIT_FAILURE);
    }
 
    // An internal node is an operator.
    switch (root->data[0]) {
        case '~': {
            // Unary operator, use right child (assumes fully parenthesized and well-formed)
            bool childValue = evaluateTree(root->right, assignments, num_assignments);
            return !childValue;
        }
        case '*': {
            bool leftValue = evaluateTree(root->left, assignments, num_assignments);
            bool rightValue = evaluateTree(root->right, assignments, num_assignments);
            return leftValue && rightValue;
        }
        case '+': {
            bool leftValue = evaluateTree(root->left, assignments, num_assignments);
            bool rightValue = evaluateTree(root->right, assignments, num_assignments);
            return leftValue || rightValue;
        }
        case '>': {
            bool leftValue = evaluateTree(root->left, assignments, num_assignments);
            bool rightValue = evaluateTree(root->right, assignments, num_assignments);
            // Implication: P -> Q is equivalent to ~P + Q
            return !leftValue || rightValue;
        }
        default:
            fprintf(stderr, "Error: Unrecognized operator '%s' in the tree.\n", root->data);
            exit(EXIT_FAILURE);
    }
}

// Recursive helper to traverse the tree and add literals to the hash table.
static void findLiteralsRecursive(TreeNode* node, HashTable* ht) {
    if (!node) return;

    // A literal is a leaf node.
    if (node->left == NULL && node->right == NULL) {
        hashTableInsert(ht, node->data);
    }

    findLiteralsRecursive(node->left, ht);
    findLiteralsRecursive(node->right, ht);
}

// Public-facing function to collect all unique literals.
int collectUniqueLiterals(TreeNode* node, char*** literals_list) {
    HashTable* ht = createHashTable();
    findLiteralsRecursive(node, ht);

    int count = 0;
    // First, count the number of unique literals
    for (int i = 0; i < HASH_TABLE_SIZE; i++) {
        for (HashNode* current = ht->table[i]; current != NULL; current = current->next) {
            count++;
        }
    }

    if (count == 0) {
        *literals_list = NULL;
        freeHashTable(ht);
        return 0;
    }

    // Allocate memory and copy the literal names from the hash table
    *literals_list = malloc(count * sizeof(char*));
    int index = 0;
    for (int i = 0; i < HASH_TABLE_SIZE; i++) {
        for (HashNode* current = ht->table[i]; current != NULL; current = current->next) {
            // Note: The literal names are duplicated from the hash table's keys
            // This is necessary because the hash table's memory will be freed.
            (*literals_list)[index++] = strdup(current->key);
        }
    }

    freeHashTable(ht);
    return count;
}

// Updated to handle string literals (Kept as provided)
TruthAssignment* getAssignmentsFromTerminal(char** literals, int count) {
    TruthAssignment* assignments = malloc(count * sizeof(TruthAssignment));
    if (!assignments) {
        perror("Failed to allocate memory for assignments");
        return NULL;
    }
    printf("Please enter truth values (T/F) for each literal:\n");

    for (int i = 0; i < count; i++) {
        char input[10];
        printf("  %s: ", literals[i]);
        if (scanf("%9s", input) != 1) {
            fprintf(stderr, "Error reading input.\n");
            // Clean up and exit or handle error
            // For simplicity, we assume valid input for now.
        }
        assignments[i].literal = strdup(literals[i]);
        assignments[i].value = (toupper(input[0]) == 'T');
    }
    return assignments;
}

// Updated to handle string literals (Kept as provided)
TruthAssignment* getAssignmentsFromFile(const char* filename, int* num_assignments) {
    FILE* file = fopen(filename, "r");
    if (!file) {
        perror("Error opening assignment file");
        return NULL;
    }

    TruthAssignment* assignments = NULL;
    char line[512]; // Increased buffer size for potentially long literals
    int count = 0;
    int capacity = 10;
    assignments = malloc(capacity * sizeof(TruthAssignment));

    // Expected format: literal_name=T or another_literal=F
    while (fgets(line, sizeof(line), file)) {
        char* literal = strtok(line, "=");
        char* value_str = strtok(NULL, "\n");

        if (literal && value_str) {
            if (count >= capacity) {
                capacity *= 2;
                TruthAssignment* new_assignments = realloc(assignments, capacity * sizeof(TruthAssignment));
                if (!new_assignments) {
                    perror("Realloc failed");
                    // Cleanup existing assignments memory before returning
                    for(int i = 0; i < count; i++) free(assignments[i].literal);
                    free(assignments);
                    fclose(file);
                    return NULL;
                }
                assignments = new_assignments;
            }
            // Trim potential whitespace
            literal[strcspn(literal, " \t\r\n")] = 0;
            
            assignments[count].literal = strdup(literal);
            assignments[count].value = (toupper(value_str[0]) == 'T');
            count++;
        }
    }

    fclose(file);
    *num_assignments = count;
    return assignments;
}


// --- New Truth Table Functionality ---

/**
 * @brief Recursive helper to generate all 2^n truth assignments and print the results.
 * @param root The root of the expression tree.
 * @param literals An array of unique literal names.
 * @param count The total number of unique literals (depth of recursion).
 * @param current_index The index of the literal currently being assigned a value.
 * @param current_assignments The array to store the current truth assignment being built.
 */
static void generateAssignmentsAndPrint(TreeNode* root, char** literals, int count, int current_index, TruthAssignment current_assignments[]) {
    // Base case: All literals have been assigned a value
    if (current_index == count) {
        // 1. Evaluate the formula with the current assignment
        bool result = evaluateTree(root, current_assignments, count);

        // 2. Print the assignment and the result
        for (int i = 0; i < count; i++) {
            printf("| %-5s ", current_assignments[i].value ? "T" : "F");
        }
        printf("| %-5s |\n", result ? "T" : "F");
        return;
    }

    // Recursive step: Try both False and True for the current literal

    // Assignment 1: Set current literal to False
    current_assignments[current_index].literal = literals[current_index];
    current_assignments[current_index].value = false;
    generateAssignmentsAndPrint(root, literals, count, current_index + 1, current_assignments);

    // Assignment 2: Set current literal to True
    current_assignments[current_index].literal = literals[current_index];
    current_assignments[current_index].value = true;
    generateAssignmentsAndPrint(root, literals, count, current_index + 1, current_assignments);
}

/**
 * @brief Generates and prints the complete truth table for the formula.
 */
void printTruthTable(TreeNode* root, char** literals, int count, const char* formula_str) {
    if (count == 0 || root == NULL) {
        printf("Cannot generate a truth table: No literals or empty formula.\n");
        return;
    }

    // 1. Print the header
    int total_width = 0;
    for (int i = 0; i < count; i++) {
        printf("| %-5s ", literals[i]);
        total_width += 8; // | LLLLL | (length 8)
    }
    printf("| %s |\n", formula_str);
    total_width += (strlen(formula_str) + 4); // | Formula |

    // Print a separator line
    for (int i = 0; i < total_width; i++) {
        printf("-");
    }
    printf("\n");

    // 2. Allocate an array to hold the current assignment during recursion
    TruthAssignment* current_assignments = malloc(count * sizeof(TruthAssignment));
    if (!current_assignments) {
        perror("Failed to allocate memory for assignments");
        return;
    }

    // 3. Start the recursive generation and printing process
    generateAssignmentsAndPrint(root, literals, count, 0, current_assignments);

    // 4. Clean up
    free(current_assignments);
}
