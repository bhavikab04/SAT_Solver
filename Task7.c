#include <stdio.h>
#include <string.h>
#include <stdbool.h>
#include <stdlib.h>
#include <ctype.h>
#include "Task7.h"

static bool checkClause(char* clause);
static char* trimWhitespace(char* str);


/**
 * @brief Checks if a given CNF formula is a tautology (always true).
 * @param formula The CNF formula to check is in string format.
 * Clauses are separated by '&'(AND), and literals within a clause
 * are separated by '|'(OR). Negation is marked with '~'(NOT).
 * Example: "p|~p & q|r"
 * @return Returns true if the formula is valid (a tautology), and false otherwise.
 *
 * A CNF formula is a tautology
 * if and only if every single one of its clauses is a tautology.
 * A clause is a tautology if it contains a literal and its negation (e.g., p and ~p).
 * We have to make a copy of the input string because the strtok function we use for
 * parsing actually modifies the string it's working on.
 */
bool isValidCNF(const char* formula) { //checks if formula is a valid cnf formula
    if (formula == NULL || *formula == '\0') {
        return true;
    }
    char* formula_copy = strdup(formula);
    if (formula_copy == NULL) {
        perror("Failed to allocate memory for formula copy");
        return false;
    }

    // Use strtok to split the formula into individual clauses using '&' as the delimiter
    char* clause_token = strtok(formula_copy, "&");

    while (clause_token != NULL) {
        if (!checkClause(clause_token)) {
            free(formula_copy); 
            return false;
        }
        clause_token = strtok(NULL, "&");
    }

    free(formula_copy); 
    return true;
}


/**
 * @brief Scans a single clause to see if it contains a complementary literal pair.
 * @param clause A string representing one clause, like "p | q | ~p".
 * @return Returns true if the clause contains a literal and its negation, false otherwise.
 *
 * The validity checker uses a
 * tracking array, `literal_state`, to keep a record of the literals it has
 * encountered so far in this clause.
 * - 0 means we haven't seen the variable.
 * - 1 means we've seen its positive form (e.g., 'p').
 * - -1 means we've seen its negative form (e.g., '~p').
 * If we ever find a literal whose opposite has already been marked, we've found a pair
 */
static bool checkClause(char* clause) {
    int literal_state[26] = {0}; // One slot for each letter from 'a' to 'z'.

    // Split the clause into literals using '|' as the delimiter.
    char* literal_token = strtok(clause, "|");

    while (literal_token != NULL) {
        char* literal = trimWhitespace(literal_token);

        if (strlen(literal) > 0) {
            int idx;
            // Case 1: The literal starts with '~'
            if (literal[0] == '~') {
                // a valid negated literal is like "~p" and has length 2.
                if (strlen(literal) != 2 || !islower(literal[1])) continue;
                idx = literal[1] - 'a'; // Find index of the given character literal in the alphabet
                if (literal_state[idx] == 1) {
                    return true; // The clause is valid cause pair found
                }
                literal_state[idx] = -1; // Mark that we've seen the negative form.

            } else { // Case 2: The literal is positive
                // a valid positive literal is just "p", length 1
                if (strlen(literal) != 1 || !islower(literal[0])) continue;
                idx = literal[0] - 'a';
                if (literal_state[idx] == -1) {
                    return true; //clause if valid cause pair found
                }
                literal_state[idx] = 1; // Mark that we've seen the positive form.
            }
        }
        // Grab the next literal from the clause string.
        literal_token = strtok(NULL, "|");
    }

    // If the loop finishes, it means we checked every literal
    // and never found a complementary pair. This clause isn't a tautology.
    return false;
}


/**
 * @brief A utility function to remove whitespace from the start and end of a string.
 * @param str The string to be trimmed
 * @return A pointer to the beginning of the trimmed string.
 *
 * This function is essential because strtok can leave extra spaces around the tokens
 * it finds. For example, splitting "p | q" would give us " p " and " q ". This
 * function cleans those up so our logic can reliably check the first character.
 */
static char* trimWhitespace(char* str) {
    char *end;

    // Move the start pointer forward past any leading spaces.
    while (isspace((unsigned char)*str)) str++;

    if (*str == 0) // If the string is all spaces, it's now empty.
        return str;

    // Move the end pointer backward past any trailing spaces.
    end = str + strlen(str) - 1;
    while (end > str && isspace((unsigned char)*end)) end--;

    // Write a new null terminator to chop off the trailing space.
    *(end + 1) = '\0';

    return str;
}
