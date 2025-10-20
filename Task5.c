#include <stdio.h>
#include <stdlib.h>
#include <stdbool.h>
#include <string.h>
#include "Task5.h"
#include "Task2.h"

#define HASH_TABLE_SIZE 2048 // Increased size for potentially many variables

// --- Private Helper Prototypes ---
static void collectLiteralsRecursive(TreeNode* node, AssignmentHashTable* ht);
static void generateAssignmentsAndPrint(TreeNode* root, char** literals, int count, int index, AssignmentHashTable* current_assignments);

// --- NEW: Hash Table specifically for Truth Assignments ---
typedef struct AssignmentNode {
    char* literal;
    bool value;
    struct AssignmentNode* next;
} AssignmentNode;

// This is the concrete implementation of the opaque pointer in the .h file
struct AssignmentHashTable {
    AssignmentNode* table[HASH_TABLE_SIZE];
};

static unsigned long hash(const char *str) {
    unsigned long hash = 5381;
    int c;
    while ((c = *str++))
        hash = ((hash << 5) + hash) + c;
    return hash % HASH_TABLE_SIZE;
}

static AssignmentHashTable* createAssignmentHashTable() {
    return (AssignmentHashTable*)calloc(1, sizeof(AssignmentHashTable));
}

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

static void assignmentHT_Insert(AssignmentHashTable* ht, const char* literal, bool value) {
    unsigned long index = hash(literal);
    // Note: This simple version doesn't check for duplicates, assuming the input file is correct.
    AssignmentNode* newNode = (AssignmentNode*)malloc(sizeof(AssignmentNode));
    newNode->literal = strdup(literal);
    newNode->value = value;
    newNode->next = ht->table[index];
    ht->table[index] = newNode;
}

// The core of the performance improvement: O(1) average lookup.
static bool assignmentHT_Get(const AssignmentHashTable* ht, const char* literal, bool* out_value) {
    unsigned long index = hash(literal);
    AssignmentNode* current = ht->table[index];
    while (current) {
        if (strcmp(current->literal, literal) == 0) {
            *out_value = current->value;
            return true; // Found
        }
        current = current->next;
    }
    return false; // Not found
}

// --- Core Public Functions (Updated) ---

// The evaluateTree function is now much faster.
bool evaluateTree(TreeNode *root, const AssignmentHashTable *assignments) {
    if (!root || !root->data) return false;

    if (isAtom(root->data)) {
        bool value;
        if (assignmentHT_Get(assignments, root->data, &value)) {
            return value;
        }
        fprintf(stderr, "Warning: No assignment for literal '%s'. Defaulting to false.\n", root->data);
        return false;
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

// This function now builds the hash table.
bool evaluateFromFile(TreeNode* root, const char* filename) {
    FILE *fp = fopen(filename, "r");
    if (!fp) {
        perror("Error opening assignments file");
        return false;
    }

    AssignmentHashTable* assignments_ht = createAssignmentHashTable();
    char line_buffer[256];
    int assignment_count = 0;

    while (fgets(line_buffer, sizeof(line_buffer), fp)) {
        if (line_buffer[0] == '#' || line_buffer[0] == '\n') continue;

        char literal_buffer[128];
        char value_char;
        if (sscanf(line_buffer, "%127s = %c", literal_buffer, &value_char) == 2) {
            bool value = (value_char == 'T' || value_char == 't');
            assignmentHT_Insert(assignments_ht, literal_buffer, value);
            assignment_count++;
        }
    }
    fclose(fp);

    if (assignment_count == 0) {
        fprintf(stderr, "Error: No valid assignments found in '%s'.\n", filename);
        freeAssignmentHashTable(assignments_ht);
        return false;
    }

    printf("Evaluating with %d assignments from '%s'...\n", assignment_count, filename);
    bool result = evaluateTree(root, assignments_ht);
    printf("\n--> Based on the input file, the formula evaluates to: %s\n", result ? "True" : "False");

    freeAssignmentHashTable(assignments_ht);
    return true;
}

// This function for collecting literals is now separate and still uses a hash table for efficiency.
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

static void collectLiteralsRecursive(TreeNode* node, AssignmentHashTable* ht) {
    if (node == NULL) return;
    if (isAtom(node->data)) {
        bool value;
        if (!assignmentHT_Get(ht, node->data, &value)) {
            assignmentHT_Insert(ht, node->data, false); // Value doesn't matter here
        }
    }
    collectLiteralsRecursive(node->left, ht);
    collectLiteralsRecursive(node->right, ht);
}


void printTruthTable(TreeNode* root, char** literals, int count, const char* formula_str) {
    if (count == 0 || !root) return;
    if (count > MAX_TRUTH_TABLE_VARIABLES) {
        printf("Skipping truth table: %d variables exceeds the configured limit of %d.\n", count, MAX_TRUTH_TABLE_VARIABLES);
        return;
    }

    // Print header...
    int total_width = 0;
    for (int i = 0; i < count; i++) {
        printf("| %-5s ", literals[i]);
        total_width += 8;
    }
    printf("| %s |\n", formula_str);
    total_width += strlen(formula_str) + 4;
    for (int i = 0; i < total_width; i++) printf("-");
    printf("\n");

    AssignmentHashTable* current_assignments = createAssignmentHashTable();
    generateAssignmentsAndPrint(root, literals, count, 0, current_assignments);
    freeAssignmentHashTable(current_assignments);
}

// Updated to use the new hash table for recursive calls.
static void generateAssignmentsAndPrint(TreeNode* root, char** literals, int count, int index, AssignmentHashTable* current_assignments) {
    if (index == count) {
        for (int i = 0; i < count; i++) {
            bool value;
            assignmentHT_Get(current_assignments, literals[i], &value);
            printf("| %-5s ", value ? "T" : "F");
        }
        bool result = evaluateTree(root, current_assignments);
        printf("| %s |\n", result ? "True" : "False");
        return;
    }

    assignmentHT_Insert(current_assignments, literals[index], false);
    generateAssignmentsAndPrint(root, literals, count, index + 1, current_assignments);

    assignmentHT_Insert(current_assignments, literals[index], true);
    generateAssignmentsAndPrint(root, literals, count, index + 1, current_assignments);
}

