#include "convertingCNFtoInput.h" // Include its own header

#include <stdlib.h>
#include <string.h>

// Define a safe buffer size for reading lines
#define LINE_BUFFER_SIZE 65536 
// Define a safe buffer size for a single literal string (e.g., "(~x2147483647)")
#define LITERAL_BUFFER_SIZE 32

/**
 * @file convertingCNFtoInput.c
 * @brief Implementation file for CNF to Infix Conversion.
 * @author [Your Name/Handle]
 * @date October 2025
 *
 * Implements the functions declared in convertingCNFtoInput.h.
 * Contains the logic to parse a .cnf file, read its clauses,
 * and build an equivalent infix string (e.g., "(x1 + -x2) * (x3)").
 */

/**
 * @brief File-scope variable to store the number of variables.
 * @note This is set by `get_clause_count()` and read by `get_variable_count()`.
 * This is a simple way to pass the value without changing function signatures.
 */
static long num_vars = 0;

/**
 * @brief Builds a fully parenthesized string for a single clause.
 * (static = private to this file)
 *
 * This function takes a line from a DIMACS file (e.g., "1 -2 3 0")
 * and converts it into a fully parenthesized infix string
 * (e.g., "((x1 + (~x2)) + x3)").
 *
 * @param line A mutable string containing the clause literals.
 * @return A dynamically allocated string (char*) containing the infix
 * representation of the clause. The caller must free this string.
 */
static char* build_clause_string(char* line) {
    // clause_formula stores the partially built clause string.
    // Example:
    // 1. Starts as NULL
    // 2. Becomes "x1"
    // 3. Becomes "(x1 + (~x2))"
    // 4. Becomes "((x1 + (~x2)) + x3)"
    char* clause_formula = NULL; // This will hold the "A" in "(A + B)"
    
    // Tokenize the line by space/tab/newline to get individual literals
    char* token = strtok(line, " \t\n"); 

    // Loop through all tokens (literals) in the line
    while (token != NULL) {
        // Convert the token (string) to a long integer
        long lit = strtol(token, NULL, 10);
        
        // The DIMACS format ends each clause with a 0
        if (lit == 0) break; // End of clause

        // --- 1. Create the string for the current literal (B) ---
        
        // Temporary buffer for the current literal's string form
        char literal_str[LITERAL_BUFFER_SIZE];
        if (lit < 0) {
            // Negative literal (e.g., -2) becomes "(~x2)"
            snprintf(literal_str, LITERAL_BUFFER_SIZE, "(~x%ld)", -lit);
        } else {
            // Positive literal (e.g., 1) becomes "x1"
            snprintf(literal_str, LITERAL_BUFFER_SIZE, "x%ld", lit);
        }

        // --- 2. Combine with the formula so far ---
        
        if (clause_formula == NULL) {
            // This is the first literal in the clause.
            // Just copy its string into clause_formula.
            clause_formula = strdup(literal_str);
            if (clause_formula == NULL) {
                fprintf(stderr, "Memory allocation failed for strdup\n");
                exit(1); // Critical error
            }
        } else {
            // This is the second or subsequent literal.
            // We need to create a new string: "(A + B)"
            // where A = clause_formula (what we have so far)
            // and   B = literal_str (the new literal)
            
            // Get lengths for memory allocation
            size_t old_len = strlen(clause_formula);
            size_t lit_len = strlen(literal_str);
            
            // Need space for: '(', A, ' + ', B, ')', '\0'
            size_t new_len = old_len + lit_len + 7; // +7 is safe (for " ( + ) \0")

            // Allocate memory for the new combined string
            char* new_clause = (char*)malloc(new_len);
            if (new_clause == NULL) {
                fprintf(stderr, "Memory allocation failed for new clause\n");
                free(clause_formula);
                exit(1); // Critical error
            }

            // Create the new string "(A + B)"
            snprintf(new_clause, new_len, "(%s + %s)", clause_formula, literal_str);
            
            // Free the old (A) string
            free(clause_formula);
            // Update clause_formula to point to the new "(A + B)" string
            clause_formula = new_clause;
        }
        
        // Get the next token (literal) from the line
        token = strtok(NULL, " \t\n");
    }
    
    // Handle edge case: empty line or line with just "0"
    if (clause_formula == NULL) {
        clause_formula = strdup(""); // Return empty string, not NULL
    }
    
    // Return the final, fully built clause string
    return clause_formula;
}

/**
 * @brief Scans the file for the 'p cnf' line and returns the number of clauses.
 * @note This function also reads the number of variables and stores it in
 * the file-scope `num_vars` variable as a side effect.
 *
 * @param filename The path to the DIMACS CNF file.
 * @return The number of clauses as reported by the 'p' line, or -1 on error.
 */
long get_clause_count(const char* filename) {
    // Open the file for the first pass (just to find the 'p' line)
    FILE* file = fopen(filename, "r");
    if (file == NULL) {
        perror("Error opening file for pass 1");
        return -1;
    }

    char line_buffer[LINE_BUFFER_SIZE]; // Buffer for reading lines
    long num_clauses = 0; // Value to return

    // Read file line by line
    while (fgets(line_buffer, sizeof(line_buffer), file)) {
        // Check if the line starts with "p cnf"
        if (line_buffer[0] == 'p' && strncmp(line_buffer, "p cnf", 5) == 0) {
            char* p; // Pointer to track strtol's progress
            
            // Read number of variables. 
            // `line_buffer + 5` skips "p cnf ".
            // `p` will point to the character *after* the variable count.
            num_vars = strtol(line_buffer + 5, &p, 10); // reads number of variables
            
            // Read number of clauses.
            // `strtol` starts reading from `p` (where the last call left off).
            num_clauses = strtol(p, NULL, 10); // Get clauses
            
            break; // Found the 'p' line, we are done
        }
    }
    
    fclose(file);
    return num_clauses;
}

/**
 * @brief Retrieves the variable count read by the last call to `get_clause_count`.
 * @return The number of variables from the 'p cnf' line.
 */
long get_variable_count(void) {
    // Return the value stored in the file-scope static variable
    return num_vars;
}

/**
 * @brief Converts a CNF file into a single-line, fully parenthesized
 * infix expression by streaming the output to out_stream.
 *
 * This function implements a streaming algorithm to avoid building the
 * entire (potentially massive) infix string in memory.
 *
 * It builds an expression of the form: (C1 * (C2 * (C3 * C4)))
 *
 * @param filename The path to the DIMACS CNF file.
 * @param num_clauses The expected number of clauses (from `get_clause_count`).
 * @param out_stream The file stream (e.g., stdout) to write the output to.
 */
void convertCnfToInfix(const char* filename, long num_clauses, FILE* out_stream) {
    // Open the file for the second pass (to process clauses)
    FILE* file = fopen(filename, "r");
    if (file == NULL) {
        perror("Error opening file for pass 2");
        return;
    }

    char line_buffer[LINE_BUFFER_SIZE]; // Buffer for reading lines
    long clauses_printed = 0; // Counter to track processed clauses

    // --- Streaming Algorithm ---
    
    // 1. Print all opening parentheses
    // For N clauses, we need N-1 opening parentheses for the form
    // (C1 * (C2 * (C3 * ... )))
    if (num_clauses > 1) {
        for (long i = 0; i < num_clauses - 1; i++) {
            fprintf(out_stream, "("); // CHANGED from printf (Comment OK)
        }
    }

    // 2. Read file line by line and print clauses + operators
    while (fgets(line_buffer, sizeof(line_buffer), file)) {
        // --- Pre-process the line ---
        char* line_start = line_buffer;
        // Skip any leading whitespace
        while (*line_start == ' ' || *line_start == '\t') line_start++;

        // Skip comments ('c'), the problem line ('p'), or empty lines
        if (line_start[0] == 'c' || line_start[0] == 'p' || 
            line_start[0] == '\n' || line_start[0] == '\0') {
            continue;
        }
        
        // --- Process the clause ---
        // If it's not a comment or 'p' line, it must be a clause.
        // Build the infix string for this single clause
        char* clause_str = build_clause_string(line_start);
        
        // If the clause was empty (e.g., "0\n"), skip it
        if (strlen(clause_str) == 0) {
            free(clause_str);
            continue;
        }

        // --- Print the clause and operators ---
        
        if (clauses_printed > 0) {
            // This is the 2nd, 3rd, ... Nth clause (C2...CN).
            // We must print the ' * ' operator *before* it.
            // This forms: (C1 * C2) or (C1 * (C2 * C3)) etc.
            fprintf(out_stream, " * "); // CHANGED from printf (Comment OK)
        }

        // Print the clause string itself (e.g., C1, or C2, etc.)
        fprintf(out_stream, "%s", clause_str); // CHANGED from printf (Comment OK)
        free(clause_str); // Free the string from build_clause_string

        if (clauses_printed > 0 && num_clauses > 1) {
            // If this was C2 or later, we just printed " * Ck".
            // Now we must close the parenthesis: "(... * Ck)"
            fprintf(out_stream, ")"); // CHANGED from printf (Comment OK)
        }
        
        clauses_printed++; // Increment the count of clauses we've printed
    }

    // Done with the file
    fclose(file);

    // --- Final validation ---
    // Check if the number of clauses we found matches the 'p' line
    if (clauses_printed != num_clauses) {
        // This is an error message, so it *should* go to stderr
        fprintf(stderr, "\nWarning: 'p' line said %ld clauses, but found %ld.\n",
                num_clauses, clauses_printed);
    }
}