#include <stdio.h>
#include <stdlib.h>
#include <string.h>
#include "Task6.h"
#include "Task2.h"
#include "Task3.h"

TreeNode *create_tree_node(char data, TreeNode *left, TreeNode *right)
{
    TreeNode *node = (TreeNode *)malloc(sizeof(TreeNode));
    if (!node)
    {
        perror("Memory allocation failed for TreeNode");
        exit(EXIT_FAILURE);
    }
    node->data = data;
    node->left = left;
    node->right = right;
    return node;
}

//  Take a deep copy of a TreeNode structure
TreeNode *copyTree(const TreeNode *root)
{
    if (!root)
        return NULL;
    TreeNode *new_node = create_tree_node(root->data, copyTree(root->left), copyTree(root->right));
    return new_node;
}

/* Negation function: returns a new NOT node (~), with the copy of phi as its right child
Convention: assume the right child is filled (consistent with lectures) */
TreeNode *negate_tree(TreeNode *phi)
{
    TreeNode *negated = create_tree_node('~', NULL, copyTree(phi));
    return negated;
}

// Check if a formula is a literal (atom or negated atom)
bool isLiteral(const TreeNode *phi)
{
    if (!phi)
        return false;
    if (isAtom(phi->data))
        return true;
    // Check if it's NOT applied directly to an ATOM
    if (phi->data == '~' && phi->right && isAtom(phi->right->data))
        return true;
    return false;
}

// For a CNF output: print the output by recursive in order traversal
void print_formula(const TreeNode *f)
{
    if (!f)
        return;

    if (isAtom(f->data))
    {
        printf("%c", f->data);
    }
    else if (f->data == '~')
    {
        printf("~");
        print_formula(f->right);
    }
    else
    { // Binary operators: AND ('*'), OR ('+'), IMP ('>')
        printf("(");
        print_formula(f->left);
        printf(" %c ", f->data);
        print_formula(f->right);
        printf(")");
    }
}

// Core Algorithms:

// Implication free/elimination function:
// Like: psi -> eta becomes NOT(psi) OR eta (as in: ~psi + eta)
//--------------1. IMPL_FREE()--------------------

TreeNode *IMPL_FREE(TreeNode *phi)
{
    if (!phi)
        return NULL;

    // Base Case: Atom
    if (isAtom(phi->data))
    {
        return copyTree(phi);
    }

    switch (phi->data)
    {
    case '~':
        // Unary operator: Recurse on the right child
        return create_tree_node('~', NULL, IMPL_FREE(phi->right));

    case '*': // AND
    case '+': // OR
        // Binary operator: Recurse on both children
        return create_tree_node(phi->data, IMPL_FREE(phi->left), IMPL_FREE(phi->right));

    case '>': // IMPLICATION: A > B -> (~A + B)
    {
        TreeNode *a_free = IMPL_FREE(phi->left);
        TreeNode *b_free = IMPL_FREE(phi->right);

        TreeNode *not_a = negate_tree(a_free); // Creates ~A

        // Result: OR node (+)
        TreeNode *result = create_tree_node('+', not_a, b_free);

        freeTree(a_free); // Free the origical since the negate_tree function made a copy

        return result;
    }
    }
    return NULL;
}

//------------------2. NNF()-------------------------
// Convert to Negation Normal form:

TreeNode *NNF(TreeNode *phi)
{
    if (!phi)
        return NULL;
    if (isLiteral(phi))
        return copyTree(phi);

    switch (phi->data)
    {
    case '*': // AND
    case '+': // OR
        // Recurse down for non-negated binary operators
        return create_tree_node(phi->data, NNF(phi->left), NNF(phi->right));

    case '~':
    {
        TreeNode *inner = phi->right;
        if (!inner)
            return NULL;

        if (inner->data == '~')
        {
            // Rule: ~~A -> A (Double Negation)
            return NNF(inner->right);
        }

        if (inner->data == '*')
        {
            // Rule: ~(A * B) -> ~A + ~B (De Morgan's Law 1)
            TreeNode *l = negate_tree(inner->left);
            TreeNode *r = negate_tree(inner->right);
            TreeNode *temp_or = create_tree_node('+', l, r);
            TreeNode *result = NNF(temp_or);
            freeTree(temp_or);
            return result;
        }

        if (inner->data == '+')
        {
            // Rule: ~(A + B) -> ~A * ~B (De Morgan's Law 2)
            TreeNode *l = negate_tree(inner->left);
            TreeNode *r = negate_tree(inner->right);
            TreeNode *temp_and = create_tree_node('*', l, r);
            TreeNode *result = NNF(temp_and);
            freeTree(temp_and);
            return result;
        }
        break;
    }
    }
    return NULL;
}

//-------------------3. DISTR()----------------------
// Distribute Or over AND
// Like: eta1 + (eta21 * eta22) -> (eta1 + eta21) * (eta1 + eta22))
TreeNode *DISTR(TreeNode *eta1, TreeNode *eta2)
{
    if (!eta1 || !eta2)
        return NULL;

    // Case 1: (eta11 * eta12) + eta2
    if (eta1->data == '*')
    {
        // (A * B) + C -> (A + C) * (B + C)
        TreeNode *a = DISTR(eta1->left, eta2);
        TreeNode *b = DISTR(eta1->right, eta2);
        return create_tree_node('*', a, b);
    }

    // Case 2: eta1 + (eta21 * eta22)
    if (eta2->data == '*')
    {
        // A + (B * C) -> (A + B) * (A + C)
        TreeNode *a = DISTR(eta1, eta2->left);
        TreeNode *b = DISTR(eta1, eta2->right);
        return create_tree_node('*', a, b);
    }

    // Case 3: Otherwise (eta1 + eta2 is a simple OR clause/literal)
    return create_tree_node('+', copyTree(eta1), copyTree(eta2));
}

// CNF Main function: applies distribution recursively
TreeNode *CNF(TreeNode *phi)
{
    if (!phi)
        return NULL;

    if (isLiteral(phi))
        return copyTree(phi);

    switch (phi->data)
    {
    case '*': // AND
        // A conjunction of CNF formulas is CNF
        return create_tree_node('*', CNF(phi->left), CNF(phi->right));

    case '+': // OR
    {
        // Apply distribution after ensuring sub-formulas are CNF
        TreeNode *l = CNF(phi->left);
        TreeNode *r = CNF(phi->right);
        TreeNode *result = DISTR(l, r);

        // DISTR returns a new tree structure, so we must free the CNF sub-results
        // (l and r are not needed anymore after DISTR consumes their structure).
        freeTree(l);
        freeTree(r);

        return result;
    }

        // All other cases (atoms, NOT) are handled by isLiteral check at the start.
    }
    return NULL;
}

// CNF Driver function: contains all steps to get a CNF expression
TreeNode *CNF_FORMULA(TreeNode *phi)
{
    if (!phi)
        return NULL;

    // 1. IMPL_FREE
    TreeNode *no_imp = IMPL_FREE(phi);
    printf("1. IMPL_FREE: ");
    print_formula(no_imp);
    printf("\n");

    // 2. NNF
    TreeNode *nnf = NNF(no_imp);
    printf("2. NNF: ");
    print_formula(nnf);
    printf("\n");

    // 3. CNF
    TreeNode *cnf = CNF(nnf);
    printf("3. CNF: ");
    print_formula(cnf);
    printf("\n");

    // Free intermediate results
    freeTree(no_imp);
    freeTree(nnf);

    return cnf;
}
