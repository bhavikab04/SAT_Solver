#include <stdio.h>
#include <string.h>
#include <stdbool.h>
#include <stdlib.h>
#include <ctype.h>
#include "Task7.h"

// Forward declarations for static functions
static bool checkClause(char* clause);
static char* trimWhitespace(char* str);

/**
 * @brief Checks if a given CNF formula is a tautology and counts valid/invalid clauses.
 * @param formula The CNF formula to check is in string format.
 * @param valid_clauses A pointer to an integer where the count of valid clauses will be stored.
 * @param invalid_clauses A pointer to an integer where the count of invalid clauses will be stored.
 * @return Returns true if the formula is a tautology (all clauses are valid), and false otherwise.
 */
bool isValidCNF(const char* formula, int* valid_clauses, int* invalid_clauses) {
    // Initialize counters to zero
    *valid_clauses = 0;
    *invalid_clauses = 0;

    if (formula == NULL || *formula == '\0') {
        return true; // An empty formula is trivially a tautology with 0 clauses.
    }

    char* formula_copy = strdup(formula);
    if (formula_copy == NULL) {
        perror("Failed to allocate memory for formula copy");
        return false;
    }

    bool is_formula_tautology = true; // Assume true until an invalid clause is found.

    // Split the formula into clauses using '&' as the delimiter
    char* clause_token = strtok(formula_copy, "&");

    while (clause_token != NULL) {
        // We need a mutable copy for checkClause since it uses strtok internally
        char* clause_for_check = strdup(clause_token);
        if (clause_for_check == NULL) {
             perror("Failed to allocate memory for clause copy");
             is_formula_tautology = false; // Cannot proceed
             break;
        }

        if (checkClause(clause_for_check)) {
            (*valid_clauses)++;
        } else {
            (*invalid_clauses)++;
            is_formula_tautology = false; // One invalid clause makes the whole formula not a tautology.
        }
        
        free(clause_for_check);
        clause_token = strtok(NULL, "&");
    }

    free(formula_copy);
    return is_formula_tautology;
}

// A simple structure to keep track of seen literals in a clause.
typedef struct {
    char* name; // The base name of the literal (e.g., "p1", "isValid")
    int state;  // 1 for positive (p), -1 for negative (~p)
} LiteralRecord;


/**
 * @brief Scans a single clause to see if it contains a complementary literal pair.
 * This version handles arbitrary literal names, not just single characters.
 * @param clause A string representing one clause, like "p1 | q_var | ~p1".
 * @return Returns true if the clause contains a literal and its negation, false otherwise.
 */
static bool checkClause(char* clause) {
    LiteralRecord* seen_literals = NULL;
    int seen_count = 0;
    bool found_pair = false;

    char* literal_token = strtok(clause, "|");

    while (literal_token != NULL) {
        char* literal = trimWhitespace(literal_token);
        
        if (strlen(literal) > 0) {
            bool is_negated = literal[0] == '~';
            char* base_name = is_negated ? literal + 1 : literal;
            int current_state = is_negated ? -1 : 1;

            // Check if the complement has been seen before
            for (int i = 0; i < seen_count; i++) {
                if (strcmp(seen_literals[i].name, base_name) == 0) {
                    if (seen_literals[i].state == -current_state) {
                        found_pair = true;
                        goto cleanup; // Exit loops and proceed to cleanup
                    }
                }
            }
            
            // If not found, add this literal to our list of seen literals
            seen_count++;
            seen_literals = realloc(seen_literals, seen_count * sizeof(LiteralRecord));
            if (seen_literals == NULL) {
                perror("Failed to reallocate memory for seen literals");
                found_pair = false; // Indicate error
                goto cleanup;
            }
            seen_literals[seen_count - 1].name = strdup(base_name);
            seen_literals[seen_count - 1].state = current_state;
        }

        literal_token = strtok(NULL, "|");
    }

cleanup:
    // Free all the memory we allocated for the literal names
    for (int i = 0; i < seen_count; i++) {
        free(seen_literals[i].name);
    }
    free(seen_literals);

    return found_pair;
}


/**
 * @brief A utility function to remove whitespace from the start and end of a string.
 * @param str The string to be trimmed.
 * @return A pointer to the beginning of the trimmed string.
 */
static char* trimWhitespace(char* str) {
    char *end;

    // Trim leading space
    while (isspace((unsigned char)*str)) str++;

    if (*str == 0) // All spaces?
        return str;

    // Trim trailing space
    end = str + strlen(str) - 1;
    while (end > str && isspace((unsigned char)*end)) end--;

    // Write new null terminator character
    *(end + 1) = '\0';

    return str;
}

