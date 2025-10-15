#include <stdio.h>
#include <stdlib.h>
#include <string.h>

// Type definitions:
typedef enum
{
    ATOM,
    NOT,
    AND,
    OR,
    IMP
} FormulaType;
// easier for switch case

typedef struct Formula
{
    FormulaType type;
    char *symbol; // For ATOMs only
    struct Formula *left;
    struct Formula *right;
} Formula;

// Major function declarations:
Formula *IMPL_FREE(Formula *phi);
Formula *NNF(Formula *phi);
Formula *DISTR(Formula *eta1, Formula *eta2);
Formula *CNF(Formula *phi);
Formula *CNF_FORMULA(Formula *phi);

// Helper functions:
/* Create a new formula node:
with arguments type, symbol, left child, right child */
Formula *create_formula(FormulaType type, const char *sym, Formula *lhs, Formula *rhs)
{
    Formula *f = (Formula *)malloc(sizeof(Formula));
    if (!f)
    {
        perror("Memory allocation failed");
        exit(EXIT_FAILURE);
    }
    f->type = type;
    // strdup() internally uses malloc() to allocate memory for the string, or allocates NULL if string is NULL
    // Must release memory when no longer needed
    f->symbol = (sym != NULL) ? strdup(sym) : NULL;
    f->left = lhs;
    f->right = rhs;
    return f;
}

// Free formula by post order recursion:
void free_formula(Formula *f)
{
    if (!f)
        return;
    free_formula(f->left);
    free_formula(f->right);
    if (f->symbol)
        free(f->symbol);
    free(f);
}

// Create a deep copy of a formula:
Formula *copy_formula(const Formula *f)
{
    if (!f)
        return NULL;
    // Pass four arguments to create_formula
    return create_formula(f->type, f->symbol, copy_formula(f->left), copy_formula(f->right));
}

// Negation function:
// convention: assume the right child is filled (consistent with lectures)
//  Hypothetical negate_formula for Task 3's convention
Formula *negate_formula(Formula *phi)
{
    // Operand in the RIGHT child, LEFT child is NULL
    return create_formula(NOT, NULL, NULL, copy_formula(phi));
}

// Check if it's a literal or not:
int is_literal(const Formula *phi)
{
    if (!phi)
        return 0;
    if (phi->type == ATOM)
        return 1;
    // Check if it's NOT applied directly to an ATOM
    if (phi->type == NOT && phi->right && phi->right->type == ATOM)
        return 1;
    return 0;
}

// Print the formula:
void print_formula(const Formula *f)
{
    if (!f)
        return;

    switch (f->type)
    {
    case ATOM:
        printf("%s", f->symbol);
        break;
    case NOT:
        printf("~");
        print_formula(f->right);
        break;
    case AND:
    case OR:
    case IMP:
        printf("(");
        print_formula(f->left);
        if (f->type == AND)
            printf(" * ");
        else if (f->type == OR)
            printf(" + ");
        else
            printf(" > ");
        print_formula(f->right);
        printf(")");
        break;
    }
}

//--------------------------Main functions start here-----------------------------------
// Implication free/elimination function:
// Like: psi -> eta becomes NOT(psi) OR eta (as in: ~psi + eta)
//--------------1. IMPL_FREE()--------------------
Formula *IMPL_FREE(Formula *phi)
{
    if (!phi)
        return NULL;

    switch (phi->type)
    {
    case ATOM:
        return copy_formula(phi);

    case NOT:
        // Input phi is a NOT, its operand is expected at phi->right (Right child).
        return create_formula(NOT, NULL, NULL, IMPL_FREE(phi->right));

    case AND:
        return create_formula(AND, NULL, IMPL_FREE(phi->left), IMPL_FREE(phi->right));

    case OR:
        return create_formula(OR, NULL, IMPL_FREE(phi->left), IMPL_FREE(phi->right));

    case IMP: // case of implication
    {
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

//------------------2. NNF()-------------------------
// Convert to Negation Normal form:
Formula *NNF(Formula *phi)
{
    if (!phi)
        return NULL;
    if (is_literal(phi))
        return copy_formula(phi);

    switch (phi->type)
    {
    case AND:
        return create_formula(AND, NULL, NNF(phi->left), NNF(phi->right));

    case OR:
        return create_formula(OR, NULL, NNF(phi->left), NNF(phi->right));

    case NOT:
    {
        Formula *inner = phi->right;
        if (!inner)
            return NULL;

        if (inner->type == NOT)
        {
            return NNF(inner->right);
        }

        if (inner->type == AND)
        {
            // ~ (A * B) -> ~A + ~B (De Morgan's 1)
            Formula *l = negate_formula(inner->left);
            Formula *r = negate_formula(inner->right);
            Formula *temp_or = create_formula(OR, NULL, l, r);
            Formula *result = NNF(temp_or);
            free_formula(temp_or);
            return result;
        }

        if (inner->type == OR)
        {
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

//-------------------3. DISTR()----------------------
// Distribute Or over AND
// Like: eta1 + (eta21 * eta22) -> (eta1 + eta21) * (eta1 + eta22))
Formula *DISTR(Formula *eta1, Formula *eta2)
{
    if (!eta1 || !eta2)
        return NULL;

    // Case 1: (eta11 * eta12) + eta2
    if (eta1->type == AND)
    {
        Formula *a = DISTR(eta1->left, eta2);
        Formula *b = DISTR(eta1->right, eta2);
        return create_formula(AND, NULL, a, b);
    }

    // Case 2: eta1 + (eta21 * eta22)
    if (eta2->type == AND)
    {
        Formula *a = DISTR(eta1, eta2->left);
        Formula *b = DISTR(eta1, eta2->right);
        return create_formula(AND, NULL, a, b);
    }

    // Case 3: Otherwise (eta1 + eta2 is a single clause)
    return create_formula(OR, NULL, eta1, eta2);
}

//------------4. CNF() (Main function)-------------
Formula *CNF(Formula *phi)
{
    if (!phi)
        return NULL;
    if (is_literal(phi))
        return copy_formula(phi);

    switch (phi->type)
    {
    case AND:
        // Conjunction of CNF formulas is CNF
        return create_formula(AND, NULL, CNF(phi->left), CNF(phi->right));

    case OR:
    {
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

//---------5. CNF_FORMULA() (Driver Function)-----------------
// Contains the entire cnf formula steps to follow
Formula *CNF_FORMULA(Formula *phi)
{
    if (!phi)
        return NULL;

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
