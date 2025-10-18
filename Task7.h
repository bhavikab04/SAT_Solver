#ifndef TASK7_H
#define TASK7_H

#include <stdbool.h> // For the bool type

/**
 * @brief Checks if a given CNF formula is a tautology and counts valid/invalid clauses.
 *
 * A CNF formula is a tautology if and only if every clause contains a complementary
 * pair of literals (e.g., p and ~p). This function checks this condition and also
 * provides counts for how many clauses meet this criteria and how many do not.
 *
 * Expected Format:
 * - Clauses are separated by '&'.
 * - Literals within a clause are separated by '|'.
 * - Negation is denoted by '~'.
 * - Literals are single lowercase alphabetic characters (a-z).
 * - Whitespace is ignored.
 * Example: "p|q|~p & ~q|r|s"
 *
 * @param formula The CNF formula as a null-terminated string.
 * @param valid_clauses A pointer to an integer to store the count of valid clauses.
 * @param invalid_clauses A pointer to an integer to store the count of invalid clauses.
 * @return True if the formula is a tautology (all clauses are valid), false otherwise.
 */
bool isValidCNF(const char* formula, int* valid_clauses, int* invalid_clauses);

#endif // TASK7_H
