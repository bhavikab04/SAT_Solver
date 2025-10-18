#include <stdio.h>
#include <stdlib.h>
#include <string.h>

// Define a safe buffer size for reading lines
#define LINE_BUFFER_SIZE 65536 
// Define a safe buffer size for a single literal string (e.g., "(~x2147483647)")
#define LITERAL_BUFFER_SIZE 32

/**
 * @brief Builds a fully parenthesized string for a single clause.
 * * Example: "1 -2 3 0" becomes "((x1 + (~x2)) + x3)"
 * Example: "1 0" becomes "x1"
 *
 * @param line The line from the CNF file containing the clause.
 * @return A new, heap-allocated string for the clause. Caller must free().
 */
char* build_clause_string(char* line) {
    char* clause_formula = NULL; // This will hold the "A" in "(A + B)"
    char* token = strtok(line, " \t\n"); // Tokenize the line by space/tab/newline

    while (token != NULL) {
        // strtol is robust and converts string to long
        long lit = strtol(token, NULL, 10);

        // A '0' terminates the clause
        if (lit == 0) {
            break;
        }

        // 1. Create the string for the current literal (B)
        char literal_str[LITERAL_BUFFER_SIZE];
        if (lit < 0) {
            // Fully parenthesize negations as (~xN)
            snprintf(literal_str, LITERAL_BUFFER_SIZE, "(~x%ld)", -lit);
        } else {
            snprintf(literal_str, LITERAL_BUFFER_SIZE, "x%ld", lit);
        }

        // 2. Combine with the formula so far
        if (clause_formula == NULL) {
            // This is the first literal in the clause.
            clause_formula = strdup(literal_str);
            if (clause_formula == NULL) {
                fprintf(stderr, "Memory allocation failed for strdup\n");
                exit(1);
            }
        } else {
            // This is the second or subsequent literal.
            // We must create: new_string = "(A + B)"
            // where A = clause_formula, B = literal_str
            
            size_t old_len = strlen(clause_formula);
            size_t lit_len = strlen(literal_str);
            // Need space for: '(', A, ' ', '+', ' ', B, ')', '\0'
            size_t new_len = 1 + old_len + 3 + lit_len + 1 + 1;

            char* new_clause = (char*)malloc(new_len);
            if (new_clause == NULL) {
                fprintf(stderr, "Memory allocation failed for new clause\n");
                free(clause_formula);
                exit(1);
            }

            // Build the new string
            snprintf(new_clause, new_len, "(%s + %s)", clause_formula, literal_str);
            
            // Free the old (A) string and replace it with the new (A + B) string
            free(clause_formula);
            clause_formula = new_clause;
        }

        // Get the next literal
        token = strtok(NULL, " \t\n");
    }
    
    // Handle edge case: empty line or line with just "0"
    if (clause_formula == NULL) {
        clause_formula = strdup(""); // Return empty string, not NULL
    }
    
    return clause_formula;
}

/**
 * @brief Converts a CNF file into a single-line, fully parenthesized
 * infix expression.
 *
 * @param filename The path to the .cnf file.
 * @return A new, heap-allocated string of the entire formula. Caller must free().
 */
char* convertCnfToInfix(const char* filename) {
    FILE* file = fopen(filename, "r");
    if (file == NULL) {
        perror("Error opening file");
        return NULL;
    }

    char line_buffer[LINE_BUFFER_SIZE];
    char* total_formula = NULL; // This will hold "A" in "(A * B)"

    // Read the file line by line
    while (fgets(line_buffer, sizeof(line_buffer), file)) {
        
        // --- Line Pre-processing ---
        // 1. Trim leading whitespace (for robustness)
        char* line_start = line_buffer;
        while (*line_start == ' ' || *line_start == '\t') {
            line_start++;
        }

        // 2. Skip comments, problem line, or empty lines
        if (line_start[0] == 'c' || line_start[0] == 'p' || 
            line_start[0] == '\n' || line_start[0] == '\0') {
            continue;
        }
        
        // --- Clause Processing ---
        // This is a clause line.
        // 1. Build the string for this clause (B)
        char* clause_str = build_clause_string(line_start);
        
        // Handle edge case: build_clause_string returned "" for an empty "0" line
        if (strlen(clause_str) == 0) {
            free(clause_str);
            continue;
        }

        // 2. Combine with the total formula so far
        if (total_formula == NULL) {
            // This is the very first clause (A).
            total_formula = clause_str; // Just assign it, don't free
        } else {
            // This is a subsequent clause.
            // We must create: new_string = "(A * B)"
            // where A = total_formula, B = clause_str

            size_t old_len = strlen(total_formula);
            size_t clause_len = strlen(clause_str);
            // Need space for: '(', A, ' ', '*', ' ', B, ')', '\0'
            size_t new_len = 1 + old_len + 3 + clause_len + 1 + 1;
            
            char* new_formula = (char*)malloc(new_len);
            if (new_formula == NULL) {
                fprintf(stderr, "Memory allocation failed for new formula\n");
                free(total_formula);
                free(clause_str);
                fclose(file);
                exit(1);
            }

            // Build the new (A * B) string
            snprintf(new_formula, new_len, "(%s * %s)", total_formula, clause_str);

            // Free the old (A) string and the (B) string
            free(total_formula);
            free(clause_str);
            
            // The new (A * B) string is now our total formula (A)
            total_formula = new_formula;
        }
    }

    fclose(file);

    // Handle edge case: file was empty or had no clauses
    if (total_formula == NULL) {
        total_formula = strdup(""); // Return empty string
    }

    return total_formula;
}


int main(int argc, char *argv[]) {
    if (argc != 2) {
        fprintf(stderr, "Usage: %s <filename.cnf>\n", argv[0]);
        return 1;
    }

    char* infix_formula = convertCnfToInfix(argv[1]);

    if (infix_formula) {
        // Print the single, massive line to stdout.
        // Your Task 1 can read this from stdin if you pipe it:
        // ./this_program test.cnf | ./task1_program
        printf("%s\n", infix_formula);
        
        // Clean up the final allocated string
        free(infix_formula);
    } else {
        fprintf(stderr, "Failed to convert file.\n");
        return 1;
    }

    return 0;
}