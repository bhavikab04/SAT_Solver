#ifndef CNF_CONVERTER_H
#define CNF_CONVERTER_H

#include <stdio.h> // Needed for FILE*

/**
 * @brief Scans the file for the 'p cnf' line and returns the number of clauses.
 *
 * @param filename The path to the .cnf file.
 * @return The number of clauses, or -1 on error.
 */
long get_clause_count(const char* filename);

/**
 * @brief Converts a CNF file into a single-line, fully parenthesized
 * infix expression and writes it to the provided stream.
 *
 * @param filename The path to the .cnf file.
 * @param num_clauses The clause count (from get_clause_count).
 * @param out_stream The file stream to write the resulting string to.
 */
void convertCnfToInfix(const char* filename, long num_clauses, FILE* out_stream);
long get_variable_count(void);

#endif // CNF_CONVERTER_H