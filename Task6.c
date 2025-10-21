/**
 * @file Task6.c
 * @brief Implementation of functions for converting propositional logic formulas to Conjunctive Normal Form (CNF).
 *
 * This file contains the implementation of functions for creating, copying, and manipulating
 * a propositional logic formula represented as a binary tree (TreeNode structure).
 * It includes the core algorithms for converting a formula to CNF:
 * 1. Implication Elimination (IMPL_FREE)
 * 2. Negation Normal Form (NNF)
 * 3. Distribution and CNF generation (CNF, DISTR)
 *
 * It is assumed that 'Task6.h', 'Task2.h', and 'Task3.h' define the TreeNode structure,
 * the 'isAtom' function, the 'freeTree' function, and the 'bool' type.
 * Operators used: '>' (Implication), '~' (Negation), '*' (AND), '+' (OR).
 */
#include <stdio.h>
#include <stdlib.h>
#include <string.h>
#include "Task6.h"
#include "Task2.h"
#include "Task3.h"

// Assume TreeNode is defined in one of the included headers, e.g., Task6.h
/**
 * @struct TreeNode
 * @brief Represents a node in the abstract syntax tree (AST) of a propositional logic formula.
 *
 * It is assumed to have the following members:
 * @var TreeNode::data
 * The operator or atom character ('A'-'Z', 'a'-'z', '~', '>', '+', '*').
 * @var TreeNode::left
 * Pointer to the left child node. Used for binary operators.
 * @var TreeNode::right
 * Pointer to the right child node. Used for both unary ('~') and binary operators.
 */
// The definition is not here, but the documentation is provided for context.

/**
 * @brief Creates and allocates memory for a new TreeNode.
 *
 * @param data The character data for the node (operator or atom).
 * @param left Pointer to the left child node.
 * @param right Pointer to the right child node.
 * @return TreeNode* A pointer to the newly created TreeNode. Exits on memory allocation failure.
 */
TreeNode *create_tree_node(char *data, TreeNode *left, TreeNode *right)
{
    TreeNode *node = (TreeNode *)malloc(sizeof(TreeNode));
    if (!node)
    {
        perror("Memory allocation failed for TreeNode");
        exit(EXIT_FAILURE);
    }
    node->data = strdup(data); // Duplicate the string data
    if (!node->data)
    {
        perror("Memory allocation failed for TreeNode data");
        free(node);
        exit(EXIT_FAILURE);
    }
    node->left = left;
    node->right = right;
    return node;
}

/**
 * @brief Creates a deep copy of a propositional logic formula tree.
 *
 * This function recursively duplicates the entire tree structure rooted at 'root'.
 *
 * @param root A constant pointer to the root of the tree to be copied.
 * @return TreeNode* A pointer to the root of the newly created deep copy, or NULL if the input is NULL.
 */
TreeNode *copyTree(const TreeNode *root)
{
    if (!root)
        return NULL;
    TreeNode *new_node = create_tree_node(root->data, copyTree(root->left), copyTree(root->right));
    return new_node;
}

/**
 * @brief Creates a new negation node ("~") for a given formula $\phi$.
 *
 * The function creates a new NOT node and assigns a deep copy of $\phi$ to its right child.
 *
 * @param phi The root of the formula to be negated.
 * @return TreeNode* A pointer to the new negation tree node, representing $\sim\phi$.
 */
TreeNode *negate_tree(TreeNode *phi)
{
    TreeNode *negated = create_tree_node("~", NULL, copyTree(phi));
    return negated;
}

/**
 * @brief Checks if a formula is a literal.
 *
 * A literal is defined as an atom (e.g., "A") or a negated atom (e.g., "~A").
 * This function assumes 'isAtom(char *data)' is defined elsewhere.
 *
 * @param phi A constant pointer to the root of the formula to check.
 * @return bool True if the formula is a literal, false otherwise.
 */
bool isLiteral(const TreeNode *phi)
{
    if (!phi)
        return false;

    // Case 1: Atom
    if (isAtom(phi->data))
        return true;
    // Case 2: Negated Atom (Check if NOT is applied directly to an ATOM)
    if (strcmp(phi->data, "~") == 0 && phi->right && isAtom(phi->right->data))
        return true;

    return false;
}

/**
 * @brief Prints the propositional logic formula represented by the tree.
 *
 * Prints the formula using infix notation for binary operators, parentheses
 * for operator precedence, and prefix notation for negation ('~').
 *
 * @param f A constant pointer to the root of the formula tree.
 */
void print_formula(const TreeNode *f)
{
    if (!f)
        return;

    if (isAtom(f->data))
    {
        printf("%s", f->data);
    }
    else if (strcmp(f->data, "~") == 0)
    {
        printf("~");
        print_formula(f->right);
    }
    else
    { // Binary operators: AND ('*'), OR ('+'), IMP ('>')
        printf("(");
        print_formula(f->left);
        printf(" %s ", f->data);
        print_formula(f->right);
        printf(")");
    }
}

// Core Algorithms:

/**
 * @brief Eliminates all implication operators ('>') from the formula.
 *
 * Applies the equivalence $\psi \rightarrow \eta \equiv \sim\psi \lor \eta$
 * (represented as `A > B` becomes `(~A + B)`) recursively.
 * The function creates and returns a new tree structure.
 *
 * @param phi The root of the formula tree.
 * @return TreeNode* The root of the new formula tree without implications.
 */
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

    // Case '~': Unary operator
    if (strcmp(phi->data, "~") == 0)
    {
        return create_tree_node("~", NULL, IMPL_FREE(phi->right));
    }

    // Case '*' and '+': Binary operators
    if (strcmp(phi->data, "*") == 0 || strcmp(phi->data, "+") == 0)
    {
        return create_tree_node(phi->data, IMPL_FREE(phi->left), IMPL_FREE(phi->right));
    }

    // Case '>' : Implication
    if (strcmp(phi->data, ">") == 0)
    {
        TreeNode *a_free = IMPL_FREE(phi->left);
        TreeNode *b_free = IMPL_FREE(phi->right);

        TreeNode *not_a = negate_tree(a_free); // Creates ~A

        // Result: OR node (+)
        TreeNode *result = create_tree_node("+", not_a, b_free);

        freeTree(a_free);
        // Free the original since a copy was already made by negate_tree function

        return result;
    }
    return NULL;
}

/**
 * @brief Converts a formula to Negation Normal Form (NNF).
 *
 * Applies rules for Double Negation ($\sim\sim A \equiv A$) and De Morgan's Laws
 * ($\sim(A \land B) \equiv \sim A \lor \sim B$, $\sim(A \lor B) \equiv \sim A \land \sim B$)
 * recursively until negations only appear immediately before atoms (literals).
 * The input formula must be implication-free.
 *
 * @param phi The root of the formula tree.
 * @return TreeNode* The root of the new formula tree in NNF.
 */
//------------------2. NNF()-------------------------
// Convert to Negation Normal form:

TreeNode *NNF(TreeNode *phi)
{
    if (!phi)
        return NULL;
    if (isLiteral(phi))
        return copyTree(phi);

    // Case AND(*), OR(+):
    if (strcmp(phi->data, "*") == 0 || strcmp(phi->data, "+") == 0)
    {
        return create_tree_node(phi->data, NNF(phi->left), NNF(phi->right));
    }
    // Case NOT(~):
    else if (strcmp(phi->data, "~") == 0)
    {
        TreeNode *inner = phi->right;
        if (!inner)
            return NULL;

        if (strcmp(inner->data, "~") == 0)
        {
            // Rule: ~~A -> A (Double Negation)
            return NNF(inner->right);
        }

        if (strcmp(inner->data, "*") == 0)
        {
            // Rule: ~(A * B) -> ~A + ~B (De Morgan's Law 1)
            TreeNode *l = negate_tree(inner->left);
            TreeNode *r = negate_tree(inner->right);
            TreeNode *temp_or = create_tree_node("+", l, r);
            TreeNode *result = NNF(temp_or);
            freeTree(temp_or);
            return result;
        }

        if (strcmp(inner->data, "+") == 0)
        {
            // Rule: ~(A + B) -> ~A * ~B (De Morgan's Law 2)
            TreeNode *l = negate_tree(inner->left);
            TreeNode *r = negate_tree(inner->right);
            TreeNode *temp_and = create_tree_node("*", l, r);
            TreeNode *result = NNF(temp_and);
            freeTree(temp_and);
            return result;
        }
    }
    return NULL;
}

/**
 * @brief Distributes disjunction ('+') over conjunction ('*').
 *
 * Applies the distributive law:
 * $A \lor (B \land C) \equiv (A \lor B) \land (A \lor C)$
 * and its symmetric form $(B \land C) \lor A \equiv (B \lor A) \land (C \lor A)$.
 * The function creates and returns a new tree structure.
 *
 * @param eta1 The left sub-formula of the disjunction.
 * @param eta2 The right sub-formula of the disjunction.
 * @return TreeNode* The root of the new formula tree after distribution.
 */
//-------------------3. DISTR()----------------------
// Distribute Or over AND
// Like: eta1 + (eta21 * eta22) -> (eta1 + eta21) * (eta1 + eta22))
TreeNode *DISTR(TreeNode *eta1, TreeNode *eta2)
{
    if (!eta1 || !eta2)
        return NULL;

    // Case 1: (eta11 * eta12) + eta2  $\equiv$ (eta11 + eta2) * (eta12 + eta2)
    if (strcmp(eta1->data, "*") == 0)
    {
        // (A * B) + C -> (A + C) * (B + C)
        TreeNode *a = DISTR(eta1->left, eta2);
        TreeNode *b = DISTR(eta1->right, eta2);
        return create_tree_node("*", a, b);
    }

    // Case 2: eta1 + (eta21 * eta22) $\equiv$ (eta1 + eta21) * (eta1 + eta22)
    if (strcmp(eta2->data, "*") == 0)
    {
        // A + (B * C) -> (A + B) * (A + C)
        TreeNode *a = DISTR(eta1, eta2->left);
        TreeNode *b = DISTR(eta1, eta2->right);
        return create_tree_node("*", a, b);
    }

    // Case 3: Otherwise (eta1 + eta2 is a simple OR clause/literal)
    // Create a new OR node with deep copies of the children.
    return create_tree_node("+", copyTree(eta1), copyTree(eta2));
}

/**
 * @brief Converts an NNF formula to Conjunctive Normal Form (CNF).
 *
 * Recursively applies the distribution of OR over AND (using DISTR) to ensure
 * the final formula is a conjunction of disjunctions of literals.
 * This is the main recursive function for the CNF transformation step.
 *
 * @param phi The root of the formula tree, assumed to be in NNF.
 * @return TreeNode* The root of the new formula tree in CNF.
 */
TreeNode *CNF(TreeNode *phi)
{
    if (!phi)
        return NULL;

    if (isLiteral(phi))
        return copyTree(phi);

    // Case AND(*):
    if (strcmp(phi->data, "*") == 0)
    {
        // AND
        // A conjunction of CNF formulas is CNF
        return create_tree_node("*", CNF(phi->left), CNF(phi->right));
    }

    // Case OR(+):
    else if (strcmp(phi->data, "+") == 0)
    {
        // OR
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

    return NULL;
}

/**
 * @brief Driver function for converting an arbitrary formula to Conjunctive Normal Form (CNF).
 *
 * This function performs the standard three-step conversion process:
 * 1. Eliminate implications (IMPL_FREE).
 * 2. Convert to Negation Normal Form (NNF).
 * 3. Distribute OR over AND to achieve CNF (CNF).
 * Prints intermediate results and frees intermediate trees.
 *
 * @param phi The root of the initial formula tree.
 * @return TreeNode* The root of the final formula tree in CNF. The caller is responsible for freeing this tree.
 */
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