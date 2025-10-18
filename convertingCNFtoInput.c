#include <stdio.h>
#include <stdlib.h>
#include <string.h>

// Define a safe buffer size for reading lines
#define LINE_BUFFER_SIZE 65536 
// Define a safe buffer size for a single literal string (e.g., "(~x2147483647)")
#define LITERAL_BUFFER_SIZE 32

/**
 * @brief Builds a fully parenthesized string for a single clause.
 * Example: "1 -2 3 0" becomes "((x1 + (~x2)) + x3)"
 *
 * @param line The line from the CNF file containing the clause.
 * @return A new, heap-allocated string for the clause. Caller must free().
 */
char* build_clause_string(char* line) {
    char* clause_formula = NULL; // This will hold the "A" in "(A + B)"
    char* token = strtok(line, " \t\n"); // Tokenize the line by space/tab/newline

    while (token != NULL) {
        long lit = strtol(token, NULL, 10);
        if (lit == 0) break; // End of clause

        // 1. Create the string for the current literal (B)
        char literal_str[LITERAL_BUFFER_SIZE];
        if (lit < 0) {
            snprintf(literal_str, LITERAL_BUFFER_SIZE, "(~x%ld)", -lit);
        } else {
            snprintf(literal_str, LITERAL_BUFFER_SIZE, "x%ld", lit);
        }

        // 2. Combine with the formula so far
        if (clause_formula == NULL) {
            // First literal in the clause.
            clause_formula = strdup(literal_str);
            if (clause_formula == NULL) {
                fprintf(stderr, "Memory allocation failed for strdup\n");
                exit(1);
            }
        } else {
            // Second or subsequent literal. Create "(A + B)"
            size_t old_len = strlen(clause_formula);
            size_t lit_len = strlen(literal_str);
            // Need space for: '(', A, ' + ', B, ')', '\0'
            size_t new_len = old_len + lit_len + 7;

            char* new_clause = (char*)malloc(new_len);
            if (new_clause == NULL) {
                fprintf(stderr, "Memory allocation failed for new clause\n");
                free(clause_formula);
                exit(1);
            }

            snprintf(new_clause, new_len, "(%s + %s)", clause_formula, literal_str);
            
            free(clause_formula);
            clause_formula = new_clause;
        }
        token = strtok(NULL, " \t\n");
    }
    
    // Handle edge case: empty line or line with just "0"
    if (clause_formula == NULL) {
        clause_formula = strdup(""); // Return empty string, not NULL
    }
    
    return clause_formula;
}

/**
 * @brief Scans the file for the 'p cnf' line and returns the number of clauses.
 */
long get_clause_count(const char* filename) {
    FILE* file = fopen(filename, "r");
    if (file == NULL) {
        perror("Error opening file for pass 1");
        return -1;
    }

    char line_buffer[LINE_BUFFER_SIZE];
    long num_clauses = 0;

    while (fgets(line_buffer, sizeof(line_buffer), file)) {
        if (line_buffer[0] == 'p' && strncmp(line_buffer, "p cnf", 5) == 0) {
            char* p;
            strtol(line_buffer + 5, &p, 10); // Skip variables
            num_clauses = strtol(p, NULL, 10); // Get clauses
            break;
        }
    }
    
    fclose(file);
    return num_clauses;
}

/**
 * @brief Converts a CNF file into a single-line, fully parenthesized
 * infix expression by streaming the output.
 */
void convertCnfToInfix(const char* filename, long num_clauses) {
    FILE* file = fopen(filename, "r");
    if (file == NULL) {
        perror("Error opening file for pass 2");
        return;
    }

    char line_buffer[LINE_BUFFER_SIZE];
    long clauses_printed = 0;

    // --- Streaming Algorithm ---
    // 1. Print all opening parentheses
    if (num_clauses > 1) {
        for (long i = 0; i < num_clauses - 1; i++) {
            printf("(");
        }
    }

    // 2. Read file and print clauses + operators
    while (fgets(line_buffer, sizeof(line_buffer), file)) {
        char* line_start = line_buffer;
        while (*line_start == ' ' || *line_start == '\t') line_start++;

        // Skip comments, problem line, or empty lines
        if (line_start[0] == 'c' || line_start[0] == 'p' || 
            line_start[0] == '\n' || line_start[0] == '\0') {
            continue;
        }
        
        // This is a clause line.
        char* clause_str = build_clause_string(line_start);
        if (strlen(clause_str) == 0) {
            free(clause_str);
            continue;
        }

        if (clauses_printed > 0) {
            // This is C2...CN. Print the operator *before* it.
            printf(" * ");
        }

        // Print the clause string (C1, C2, etc.)
        printf("%s", clause_str);
        free(clause_str);

        if (clauses_printed > 0 && num_clauses > 1) {
            // Close the parenthesis for the ' * '
            printf(")");
        }
        
        clauses_printed++;
    }

    fclose(file);

    if (clauses_printed != num_clauses) {
        fprintf(stderr, "\nWarning: 'p' line said %ld clauses, but found %ld.\n",
                num_clauses, clauses_printed);
    }
}


int main(int argc, char *argv[]) {
    if (argc != 2) {
        fprintf(stderr, "Usage: %s <filename.cnf>\n", argv[0]);
        return 1;
    }

    // Pass 1: Get the clause count
    long num_clauses = get_clause_count(argv[1]);
    if (num_clauses <= 0) {
        fprintf(stderr, "Error: Could not find 'p cnf ...' header or num_clauses=0.\n");
        return 1;
    }

    // Pass 2: Stream and print the infix formula
    convertCnfToInfix(argv[1], num_clauses);
    printf("\n"); // Add a final newline

    return 0;
}