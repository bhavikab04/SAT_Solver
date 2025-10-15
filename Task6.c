#include <stdio.h>
#include <stdlib.h>
#include <string.h>

// --- Type Definitions ---
typedef enum {
    ATOM,
    NOT,
    AND,
    OR,
    IMP // Implication
} FormulaType;

typedef struct Formula {
    FormulaType type;
    char *symbol; // For ATOMs only (e.g., "p", "q")
    struct Formula *left;
    struct Formula *right;
} Formula;

// --- Function Declarations (Forward) ---
Formula* IMPL_FREE(Formula *phi);
Formula* NNF(Formula *phi);
Formula* DISTR(Formula *eta1, Formula *eta2);
Formula* CNF(Formula *phi);
Formula* CNF_FORMULA(Formula *phi);

// --- Memory and Utility Helpers ---

/**
 * @brief Creates a new formula node.
 * Arguments must be: Type, Symbol, Left Child, Right Child.
 */
Formula* create_formula(FormulaType type, const char *s, Formula *l, Formula *r) {
    Formula f = (Formula)malloc(sizeof(Formula));
    if (f == NULL) {
        perror("Memory allocation failed");
        exit(EXIT_FAILURE);
    }
    f->type = type;
    // strdup allocates memory for the string, or NULL if s is NULL
    f->symbol = (s != NULL) ? strdup(s) : NULL;
    f->left = l;
    f->right = r;
    return f;
}

/**
 * @brief Frees the memory for a formula and its subformulas.
 */
void free_formula(Formula *f) {
    if (f == NULL) return;
    free_formula(f->left);
    free_formula(f->right);
    if (f->symbol) free(f->symbol);
    free(f);
}

/**
 * @brief Creates a deep copy of a formula (essential as we modify the AST recursively).
 */
Formula* copy_formula(const Formula *f) {
    if (f == NULL) return NULL;
    // Ensure all 4 arguments are passed to create_formula
    return create_formula(f->type, f->symbol, copy_formula(f->left), copy_formula(f->right));
}

/**
 * @brief Returns a NOT formula for the given phi.
 */
Formula* negate_formula(Formula *phi) {
    if (phi == NULL) return NULL;
    // Creates a new NOT node, using a copy of phi as the left child.
    return create_formula(NOT, NULL, copy_formula(phi), NULL);
}

/**
 * @brief Check if a formula is a literal (atom or negated atom).
 */
int is_literal(const Formula *phi) {
    if (phi == NULL) return 0;
    if (phi->type == ATOM) return 1;
    if (phi->type == NOT && phi->left != NULL && phi->left->type == ATOM) return 1;
    return 0;
}

/**
 * @brief Prints the formula for output/debugging.
 */
void print_formula(const Formula *f) {
    if (f == NULL) return;
    
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
            printf(f->type == AND ? " ^ " : (f->type == OR ? " V " : " -> "));
            print_formula(f->right);
            printf(")");
            break;
    }
}

// -----------------------------------------------------------------------------
// 1. IMPL_FREE (Eliminate Implication: A -> B becomes ~A V B)
// -----------------------------------------------------------------------------
Formula* IMPL_FREE(Formula *phi) {
    if (phi == NULL) return NULL;

    switch (phi->type) {
        case ATOM:
            return copy_formula(phi);
        
        case NOT:
            // CORRECTED: Pass the result of recursive IMPL_FREE(phi->left) as the left child
            return create_formula(NOT, NULL, IMPL_FREE(phi->left), NULL);

        case AND:
            return create_formula(AND, NULL, IMPL_FREE(phi->left), IMPL_FREE(phi->right));

        case OR:
            return create_formula(OR, NULL, IMPL_FREE(phi->left), IMPL_FREE(phi->right));
        
        case IMP:
            {
                // A -> B  becomes  ~A V B
                Formula *left_free = IMPL_FREE(phi->left);
                Formula *right_free = IMPL_FREE(phi->right);
                Formula *neg_left = negate_formula(left_free);
                
                // IMPORTANT: negate_formula already copied and attached left_free. 
                // We must free the temporary left_free and right_free nodes if they are no longer used.
                // In this implementation, negate_formula copies left_free, and right_free is used directly.
                // However, for consistency and safety: we will create the formula and rely on full cleanup later.
                
                // create_formula(type, symbol, left_child, right_child)
                Formula *result = create_formula(OR, NULL, neg_left, right_free);
                free_formula(left_free); // Free the intermediate result
                return result;
            }
    }
    return NULL;
}

// -----------------------------------------------------------------------------
// 2. NNF (Negation Normal Form: Push ~ inwards)
// -----------------------------------------------------------------------------
Formula* NNF(Formula *phi) {
    if (phi == NULL) return NULL;

    if (is_literal(phi)) {
        return copy_formula(phi);
    }
    
    switch (phi->type) {
        case AND:
            return create_formula(AND, NULL, NNF(phi->left), NNF(phi->right));

        case OR:
            return create_formula(OR, NULL, NNF(phi->left), NNF(phi->right));
        
        case NOT:
            {
                Formula *inner = phi->left;
                if (inner == NULL) return NULL;

                if (inner->type == NOT) {
                    // ~ (~A) -> A (Double negation)
                    return NNF(inner->left); 
                }
                
                if (inner->type == AND) {
                    // ~ (A ^ B) -> ~A V ~B (De Morgan's)
                    Formula *neg_left = negate_formula(inner->left);
                    Formula *neg_right = negate_formula(inner->right);
                    Formula *or_node = create_formula(OR, NULL, neg_left, neg_right);
                    return NNF(or_node);
                }
                
                if (inner->type == OR) {
                    // ~ (A V B) -> ~A ^ ~B (De Morgan's)
                    Formula *neg_left = negate_formula(inner->left);
                    Formula *neg_right = negate_formula(inner->right);
                    Formula *and_node = create_formula(AND, NULL, neg_left, neg_right);
                    return NNF(and_node);
                }
            }
        case ATOM: 
        case IMP: 
            return copy_formula(phi); 
    }
    return NULL;
}

// -----------------------------------------------------------------------------
// 3. DISTR (Distribute OR over AND)
// -----------------------------------------------------------------------------
Formula* DISTR(Formula *eta1, Formula *eta2) {
    if (eta1 == NULL || eta2 == NULL) return NULL;

    // Case 1: (eta11 ^ eta12) V eta2  -> (eta11 V eta2) ^ (eta12 V eta2)
    if (eta1->type == AND) {
        Formula *d1 = DISTR(eta1->left, eta2);
        Formula *d2 = DISTR(eta1->right, eta2);
        return create_formula(AND, NULL, d1, d2);
    }

    // Case 2: eta1 V (eta21 ^ eta22) -> (eta1 V eta21) ^ (eta1 V eta22)
    if (eta2->type == AND) {
        Formula *d1 = DISTR(eta1, eta2->left);
        Formula *d2 = DISTR(eta1, eta2->right);
        return create_formula(AND, NULL, d1, d2);
    }

    // Case 3: Otherwise (eta1 V eta2 is a single clause/literal)
    // eta1 and eta2 are already in CNF/Clause form, so just create the OR node.
    return create_formula(OR, NULL, eta1, eta2);
}

// -----------------------------------------------------------------------------
// 4. CNF (Main Conversion Routine)
// -----------------------------------------------------------------------------
Formula* CNF(Formula *phi) {
    if (phi == NULL) return NULL;

    if (is_literal(phi)) {
        return copy_formula(phi);
    }
    
    switch (phi->type) {
        case AND:
            // phi1 ^ phi2 -> CNF(phi1) ^ CNF(phi2)
            return create_formula(AND, NULL, CNF(phi->left), CNF(phi->right));
        
        case OR:
            // phi1 V phi2 -> DISTR(CNF(phi1), CNF(phi2))
            {
                Formula *cnf_left = CNF(phi->left);
                Formula *cnf_right = CNF(phi->right);
                Formula *result = DISTR(cnf_left, cnf_right);

                // No need to free cnf_left/right as they are attached to the result node by DISTR
                return result;
            }
        
        case ATOM: 
        case NOT: 
        case IMP: 
            return copy_formula(phi); 
    }
    return NULL;
}

// -----------------------------------------------------------------------------
// 5. CNF_FORMULA (Driver Function)
// -----------------------------------------------------------------------------
Formula* CNF_FORMULA(Formula *phi) {
    if (phi == NULL) return NULL;

    // Step 1: IMPL_FREE
    Formula *impl_free_phi = IMPL_FREE(phi);
    printf("1. IMPL_FREE: "); 
    print_formula(impl_free_phi);
    printf("\n");

    // Step 2: NNF
    Formula *nnf_phi = NNF(impl_free_phi);
    printf("2. NNF: "); 
    print_formula(nnf_phi);
    printf("\n");

    // Step 3: CNF (with DISTR)
    Formula *cnf_phi = CNF(nnf_phi);
    printf("3. CNF: "); 
    print_formula(cnf_phi);
    printf("\n");

    // Free intermediate results
    free_formula(impl_free_phi);
    free_formula(nnf_phi);

    return cnf_phi;
}


// -----------------------------------------------------------------------------
// Main Example (Exercise from slides: r -> (s -> (t ^ s -> r)))
// -----------------------------------------------------------------------------
int main() {
    // Build the formula: r -> (s -> ((t ^ s) -> r))

    // Note: We build the original formula bottom-up. The original AST must be freed later.

    // 1. t ^ s
    Formula *t_and_s = create_formula(AND, NULL, 
                                     create_formula(ATOM, "t", NULL, NULL), 
                                     create_formula(ATOM, "s", NULL, NULL));
    
    // 2. (t ^ s) -> r
    Formula *part1 = create_formula(IMP, NULL, t_and_s, 
                                    create_formula(ATOM, "r", NULL, NULL));
    
    // 3. s -> ((t ^ s) -> r)
    Formula *part2 = create_formula(IMP, NULL, 
                                    create_formula(ATOM, "s", NULL, NULL), 
                                    part1);

    // 4. r -> (s -> ((t ^ s) -> r))
    Formula *phi = create_formula(IMP, NULL, 
                                  create_formula(ATOM, "r", NULL, NULL), 
                                  part2);

    printf("Original Formula: ");
    print_formula(phi);
    printf("\n\n");
    
    Formula *cnf_result = CNF_FORMULA(phi);
    
    // Clean up all memory
    free_formula(phi);          // Free the original formula
    free_formula(cnf_result);   // Free the final CNF result

    return 0;
}
