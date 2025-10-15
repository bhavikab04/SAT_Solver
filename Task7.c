#include <stdio.h>
#include <string.h>
#include <stdbool.h>
#include <stdlib.h>
#include <ctype.h>

// --- Forward Declarations ---
bool checkClause(char* clause);
char* trimWhitespace(char* str);


bool isValidCNF(const char* formula) {
    if (formula == NULL || *formula == '\0') {
        // An empty formula can be considered trivially valid.
        return true;
    }

    // Create a mutable copy of the formula string because strtok modifies it.
    char* formula_copy = strdup(formula);
    if (formula_copy == NULL) {
        perror("Failed to allocate memory");
        return false;
    }

    // Use strtok to split the formula into clauses based on the '&' delimiter.
    char* clause_token = strtok(formula_copy, "&");

    while (clause_token != NULL) {
        // For each clause, check if it contains a complementary pair.
        if (!checkClause(clause_token)) {
            // If any clause is not a tautology, the entire formula is not valid.
            free(formula_copy); // Clean up the allocated memory
            return false;
        }
        // Get the next clause.
        clause_token = strtok(NULL, "&");
    }

    free(formula_copy); // Clean up the allocated memory
    // If all clauses were checked and passed, the formula is valid.
    return true;
}


bool checkClause(char* clause) {
    // We use an array to track the state of each possible literal (a-z).
    // 0: not seen
    // 1: seen as a positive literal (e.g., 'p')
    //-1: seen as a negative literal (e.g., '~p')
    int literal_state[26] = {0};

    // Split the clause into literals based on the '|' delimiter.
    char* literal_token = strtok(clause, "|");

    while (literal_token != NULL) {
        
        char* literal = trimWhitespace(literal_token);

        if (strlen(literal) > 0) {
            int idx;
            // Check if the literal is negated.
            if (literal[0] == '~') {
                if (strlen(literal) != 2 || !islower(literal[1])) continue; // checks if length of negated literal is not 2 and if the letter in the literal is lowercase
                idx = literal[1] - 'a'; // Get index in the 'a'-'z' alphabet
                // Check if the positive counterpart was already seen.
                if (literal_state[idx] == 1) {
                    return true; // Complementary pair found!
                }
                literal_state[idx] = -1; // Mark as seen negative.
            } else { // The literal is positive.
                if (strlen(literal) != 1 || !islower(literal[0])) continue; // checks if length of positive literal is not 1 and if the letter in the literal is lowercase
                idx = literal[0] - 'a'; // Get index for 'a'-'z'
                // Check if the negative counterpart was already seen.
                if (literal_state[idx] == -1) {
                    return true; // Complementary pair found!
                }
                literal_state[idx] = 1; // Mark as seen positive.
            }
        }
        // Get the next literal in the clause.
        literal_token = strtok(NULL, "|");
    }

    // If we iterate through all literals and find no complementary pair,
    // this clause is not a tautology.
    return false;
}


char* trimWhitespace(char* str) {
    char *end;

    // Trim leading space
    while (isspace((unsigned char)*str)) str++;

    if (*str == 0) // All spaces?
        return str;

    // Trim trailing space
    end = str + strlen(str) - 1;
    while (end > str && isspace((unsigned char)*end)) end--;

    // Write new null terminator
    *(end + 1) = '\0';

    return str;
}


