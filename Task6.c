#include <stdio.h>
#include <stdlib.h>
#include <string.h>

// -----------------------------
// Type Definitions
// -----------------------------
typedef enum {
    ATOM,
    NOT,
    AND,
    OR,
    IMP
} FormulaType;

typedef struct Formula {
    FormulaType type;
    char *symbol;   // For ATOMs only
    struct Formula *left;
    struct Formula *right;
} Formula;

// Function declarations (for the algorithm steps)
Formula* IMPL_FREE(Formula *phi);
Formula* NNF(Formula *phi);
Formula* DISTR(Formula *eta1, Formula *eta2);
Formula* CNF(Formula *phi);
Formula* CNF_FORMULA(Formula *phi);

// -----------------------------
// Helper Functions (Memory Management and Utilities)
// -----------------------------

/**
 * @brief Creates a new formula node.
 * Arguments must be: Type, Symbol, Left Child, Right Child.
 */
Formula* create_formula(FormulaType type, const char *sym, Formula *l, Formula *r) {
    Formula f = (Formula) malloc(sizeof(Formula));
    if (!f) {
        perror("Memory allocation failed");
        exit(EXIT_FAILURE);
    }
    f->type = type;
    // strdup allocates memory for the string, or NULL if s is NULL
    f->symbol = (sym != NULL) ? strdup(sym) : NULL;
    f->left = l;
    f->right = r;
    return f;
}

/**
 * @brief Frees the memory for a formula and its subformulas recursively.
 */
void free_formula(Formula *f) {
    if (!f) return;
    free_formula(f->left);
    free_formula(f->right);
    if (f->symbol) free(f->symbol);
    free(f);
}

/**
 * @brief Creates a deep copy of a formula (essential for generating new sub-trees).
 */
Formula* copy_formula(const Formula *f) {
    if (!f) return NULL;
    // Pass four arguments to create_formula
    return create_formula(f->type, f->symbol, copy_formula(f->left), copy_formula(f->right));
}

/**
 * @brief Creates a NOT formula for the given phi by copying the content.
 */
Formula* negate_formula(Formula *phi) {
    if (!phi) return NULL;
    // Creates a new NOT node, using a deep copy of phi as the left child.
    return create_formula(NOT, NULL, copy_formula(phi), NULL);
}

/**
 * @brief Check if a formula is a literal (atom or negated atom).
 */
int is_literal(const Formula *phi) {
    if (!phi) return 0;
    if (phi->type == ATOM) return 1;
    // Check if it's NOT applied directly to an ATOM
    if (phi->type == NOT && phi->left && phi->left->type == ATOM) return 1;
    return 0;
}

/**
 * @brief Prints the formula using the requested symbols: ~, +, *, >.
 */
void print_formula(const Formula *f) {
    if (!f) return;

    switch (f->type) {
        case ATOM:
            printf("%s", f->symbol);
            break;
        case NOT:
            printf("~");
            print_formula(f->left);
            break;
        case AND:
        case OR:
        case IMP:
            printf("(");
            print_formula(f->left);
            // MODIFICATION HERE: Use +, *, >
            if (f->type == AND) printf(" * ");
            else if (f->type == OR) printf(" + ");
            else printf(" > ");
            print_formula(f->right);
            printf(")");
            break;
    }
}

// -----------------------------
// 1. IMPL_FREE (Eliminate Implication: psi -> eta becomes ~psi + eta)
// -----------------------------
Formula* IMPL_FREE(Formula *phi) {
    if (!phi) return NULL;

    switch (phi->type) {
        case ATOM:
            return copy_formula(phi);

        case NOT:
            return create_formula(NOT, NULL, IMPL_FREE(phi->left), NULL);

        case AND:
            return create_formula(AND, NULL, IMPL_FREE(phi->left), IMPL_FREE(phi->right));

        case OR:
            return create_formula(OR, NULL, IMPL_FREE(phi->left), IMPL_FREE(phi->right));

        case IMP: {
            // psi -> eta becomes (~psi + eta)
            Formula *psi_free = IMPL_FREE(phi->left);
            Formula *eta_free = IMPL_FREE(phi->right);
            
            // Create the ~psi part
            Formula *not_psi = negate_formula(psi_free); 
            
            // Create the final OR node
            Formula *result = create_formula(OR, NULL, not_psi, eta_free);

            // Free the copy created by IMPL_FREE which was copied again in negate_formula
            free_formula(psi_free);
            
            return result;
        }
    }
    return NULL;
}

// -----------------------------
// 2. NNF (Convert to Negation Normal Form)
// -----------------------------
Formula* NNF(Formula *phi) {
    if (!phi) return NULL;
    if (is_literal(phi)) return copy_formula(phi);

    switch (phi->type) {
        case AND:
            return create_formula(AND, NULL, NNF(phi->left), NNF(phi->right));

        case OR:
            return create_formula(OR, NULL, NNF(phi->left), NNF(phi->right));

        case NOT: {
            Formula *inner = phi->left;
            if (!inner) return NULL;

            if (inner->type == NOT) {
                // ~ (~A) -> A
                return NNF(inner->left);
            }

            if (inner->type == AND) {
                // ~ (A * B) -> ~A + ~B (De Morgan's 1)
                Formula *l = negate_formula(inner->left);
                Formula *r = negate_formula(inner->right);
                Formula *temp_or = create_formula(OR, NULL, l, r);
                Formula *result = NNF(temp_or);
                free_formula(temp_or);
                return result;
            }

            if (inner->type == OR) {
                // ~ (A + B) -> ~A * ~B (De Morgan's 2)
                Formula *l = negate_formula(inner->left);
                Formula *r = negate_formula(inner->right);
                Formula *temp_and = create_formula(AND, NULL, l, r);
                Formula *result = NNF(temp_and);
                free_formula(temp_and);
                return result;
            }
            break; 
        }
        case ATOM: 
        case IMP: 
            return copy_formula(phi); 
    }
    return NULL;
}

// -----------------------------
// 3. DISTR (Distribute OR over AND: eta1 + (eta21 * eta22) -> (eta1 + eta21) * (eta1 + eta22))
// -----------------------------
Formula* DISTR(Formula *eta1, Formula *eta2) {
    if (!eta1 || !eta2) return NULL;

    // Case 1: (eta11 * eta12) + eta2
    if (eta1->type == AND) {
        Formula *a = DISTR(eta1->left, eta2);
        Formula *b = DISTR(eta1->right, eta2);
        return create_formula(AND, NULL, a, b);
    }

    // Case 2: eta1 + (eta21 * eta22)
    if (eta2->type == AND) {
        Formula *a = DISTR(eta1, eta2->left);
        Formula *b = DISTR(eta1, eta2->right);
        return create_formula(AND, NULL, a, b);
    }

    // Case 3: Otherwise (eta1 + eta2 is a single clause)
    return create_formula(OR, NULL, eta1, eta2);
}

// -----------------------------
// 4. CNF (Main Conversion Routine)
// -----------------------------
Formula* CNF(Formula *phi) {
    if (!phi) return NULL;
    if (is_literal(phi)) return copy_formula(phi);

    switch (phi->type) {
        case AND:
            // Conjunction of CNF formulas is CNF
            return create_formula(AND, NULL, CNF(phi->left), CNF(phi->right));

        case OR: {
            // Apply distribution after converting sub-formulas to CNF
            Formula *l = CNF(phi->left);
            Formula *r = CNF(phi->right);
            return DISTR(l, r);
        }

        case ATOM: 
        case NOT: 
        case IMP: 
            return copy_formula(phi); 
    }
    return NULL;
}

// -----------------------------
// 5. CNF_FORMULA (Driver Function)
// -----------------------------
Formula* CNF_FORMULA(Formula *phi) {
    if (!phi) return NULL;

    // 1. IMPL_FREE
    Formula *no_imp = IMPL_FREE(phi);
    printf("1. IMPL_FREE: "); 
    print_formula(no_imp);
    printf("\n");

    // 2. NNF
    Formula *nnf = NNF(no_imp);
    printf("2. NNF: "); 
    print_formula(nnf);
    printf("\n");

    // 3. CNF
    Formula *cnf = CNF(nnf);
    printf("3. CNF: "); 
    print_formula(cnf);
    printf("\n");

    // Free intermediate results
    free_formula(no_imp);
    free_formula(nnf);

    return cnf;
}


// -----------------------------
// Main Example (Exercise: r > (s > (t * s > r)))
// -----------------------------
int main() {
    // Build the formula: r > (s > ((t * s) > r))

    // 1. t * s
    Formula *t_and_s = create_formula(AND, NULL, 
                                     create_formula(ATOM, "t", NULL, NULL), 
                                     create_formula(ATOM, "s", NULL, NULL));
    
    // 2. (t * s) > r
    Formula *part1 = create_formula(IMP, NULL, t_and_s, 
                                    create_formula(ATOM, "r", NULL, NULL));
    
    // 3. s > ((t * s) > r)
    Formula *part2 = create_formula(IMP, NULL, 
                                    create_formula(ATOM, "s", NULL, NULL), 
                                    part1);

    // 4. r > (s > ((t * s) > r))
    Formula *phi = create_formula(IMP, NULL, 
                                  create_formula(ATOM, "r", NULL, NULL), 
                                  part2);

    printf("Original Formula: ");
    print_formula(phi);
    printf("\n\n");
    
    Formula *cnf_result = CNF_FORMULA(phi);
    
    // Clean up all memory
    free_formula(phi);          // Free the original input
    free_formula(cnf_result);   // Free the final CNF result

    return 0;
}
