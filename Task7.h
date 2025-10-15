#ifndef CNF_VALIDATOR_H
#define CNF_VALIDATOR_H

#include <stdbool.h> // For the bool type
/**
 * @brief Checks if a given CNF formula string is valid.
 *
 * A CNF formula is valid if and only if every clause contains a complementary
 * pair of literals (e.g., p and ~p).
 *
 * Expected Format:
 * - Clauses are separated by '&'.
 * - Literals within a clause are separated by '|'.
 * - Negation is denoted by '~'.
 * - Literals are single lowercase alphabetic characters (a-z).
 * - Whitespace is ignored.
 * Example: "p|q|~p & ~q|r|q"
 *
 * @param formula The CNF formula as a null-terminated string.
 * @return True if the formula is valid, false otherwise.
 */
bool isValidCNF(const char* formula);

#endif // CNF_VALIDATOR_H
