/**
 * \file main.c
 * \brief Main driver for the logic formula processing pipeline.
 * \author [Your Name/Handle]
 * \date October 2025
 *
 * \details This program serves as the central controller for a multi-task
 * project. It takes a single file in DIMACS CNF (Conjunctive Normal Form)
 * format as input and runs it through a series of transformations and analyses:
 *
 * 1.  **CNF to Infix**: Reads a `.cnf` file and converts it into a single
 * infix propositional logic string (using `convertingCNFtoInput.h`).
 * 2.  **Infix to Prefix**: Converts the infix string into a prefix notation
 * stack (Task 1).
 * 3.  **Prefix to Tree**: Builds a binary parse tree from the prefix
 * stack (Task 2).
 * 4.  **Tree to Infix**: Reconstructs the infix string from the parse tree
 * (Task 3).
 * 5.  **Tree Height**: Calculates the height of the parse tree (Task 4).
 * 6.  **Evaluation**: Evaluates the formula from a file and generates a
 * full truth table (Task 5).
 * 7.  **Tree to CNF**: Converts the parse tree into an equivalent CNF
 * tree structure (Task 6).
 * 8.  **CNF Validity**: Checks the resulting CNF formula for tautological
 * clauses (Task 7).
 *
 * \see convertingCNFtoInput.h, Task1.h, Task2.h, Task3.h, Task4.h, Task5.h, Task6.h, Task7.h
 */

#include <stdio.h>
#include <stdbool.h>
#include <stdlib.h>
#include <string.h>

// Include all task headers
#include "convertingCNFtoInput.h" ///< For get_clause_count, convertCnfToInfix
#include "Task1.h"                ///< For Stack, read_line, task1_infixToPrefix
#include "Task2.h"                ///< For TreeNode, prefixToTree, printTreeVertical, freeTree
#include "Task3.h"                ///< For getExpLength, inOrderTraversal
#include "Task4.h"                ///< For calculateHeight
#include "Task5.h"                ///< For evaluateFromFile, collectUniqueLiterals, printTruthTable, evaluateTree
#include "Task6.h"                ///< For CNF_FORMULA, print_formula
#include "Task7.h"                ///< For checkCNFValidity

/**
 * \brief Main entry point for the logic formula processing pipeline.
 * \param[in] argc The number of command-line arguments.
 * \param[in] argv An array of command-line argument strings.
 * \return int Returns 0 on successful completion, 1 on error.
 *
 * \details Expects exactly one argument: the path to a .cnf file.
 * It then orchestrates the entire pipeline from CNF to validity checking.
 */
int main(int argc, char *argv[])
{
    // --- Argument Check ---
    /**
     * \brief Validates the command-line arguments.
     * \details Ensures exactly one argument (the filename) is provided.
     */
    if (argc != 2)
    {
        fprintf(stderr, "Usage: %s <filename.cnf>\n", argv[0]);
        fprintf(stderr, "This will run the CNF->Infix->Prefix->... pipeline.\n");
        return 1; ///< Exit with error code 1
    }
    const char *cnf_filename = argv[1]; ///< Store the input filename
    char *buffer = NULL;                ///< This will hold the infix string generated from the CNF

    printf("--- Main Test Function ---\n");
    printf("Processing CNF file: %s\n", cnf_filename);

    // ==========================================================
    // STEP 1: CNF to Infix Conversion
    // ==========================================================
    long num_clauses = get_clause_count(cnf_filename);
    if (num_clauses <= 0)
    {
        fprintf(stderr, "Error: Could not find 'p cnf' header in %s\n", cnf_filename);
        return 1;
    }

    /**
     * \brief Use a temporary in-memory file stream.
     * \details This is an efficient way to pass the output of
     * convertCnfToInfix (which expects a FILE*) as input to
     * read_line (which also reads from a FILE*).
     * The file is automatically deleted on fclose().
     */
    FILE *temp_stream = tmpfile();
    if (temp_stream == NULL)
    {
        perror("Error creating temporary file");
        return 1;
    }

    // 1. Run CNF converter, writing its infix string to the temp file
    convertCnfToInfix(cnf_filename, num_clauses, temp_stream);

    // 2. Rewind the temp file to read from the beginning
    rewind(temp_stream);

    // 3. Use task1's read_line to read the infix string into 'buffer'
    buffer = read_line(temp_stream);
    fclose(temp_stream); ///< Temp file is auto-deleted

    if (buffer == NULL || strlen(buffer) == 0)
    {
        fprintf(stderr, "Error: CNF conversion produced no output.\n");
        free(buffer);
        return 1;
    }

    printf("Generated Infix: %s\n", buffer);

    // ==========================================================
    // STEP 2 & 3: Infix to Prefix (Task 1) & Prefix to Tree (Task 2)
    // ==========================================================
    Stack *prefix_stack = task1_infixToPrefix(buffer);

    TreeNode *root = NULL; ///< Declare the parse tree root in the outer scope

    if (prefix_stack)
    {
        printf("\n--- Task 1 Result (Prefix Stack) & Task 2 (Tree Build) ---\n");

        root = prefixToTree(prefix_stack); ///< Build the tree from the stack

        if (root)
        {
            printTreeVertical(root); ///< Print the resulting tree
        }
        else
        {
            printf("Failed to build the parse tree from the stack.\n");
        }

        ///< Always free the prefix stack after attempting to build the tree.
        freeStack(prefix_stack);
    }
    else
    {
        printf("Failed to convert the infix expression to a stack.\n");
    }

    // Abort if tree building failed, as all subsequent tasks depend on it.
    if (root == NULL)
    {
        printf("\nAborting further tasks because the parse tree could not be built.\n");
        free(buffer); ///< Free the infix buffer before exiting
        free(root);   ///< Free root (it's NULL, but safe)
        return 1;
    }

    // ==========================================================
    // STEP 4: Infix Reconstruction (Task 3)
    // ==========================================================
    printf("\n\n--- Task 3: Infix Expression Reconstruction ---\n");
    int bufferLength = getExpLength(root);          ///< Calculate needed buffer size
    char *infix = (char *)malloc(bufferLength + 1); ///< Allocate buffer
    if (infix)
    {
        int pos = 0;                         ///< Position index for the infix string
        inOrderTraversal(root, infix, &pos); ///< Rebuild string via in-order traversal
        infix[pos] = '\0';                   ///< Null-terminate the string
        printf("Reconstructed Infix: %s\n", infix);
        free(infix); ///< Free the reconstructed string
    }
    else
    {
        printf("Memory allocation failed for infix buffer string\n");
    }

    // ==========================================================
    // STEP 5: Tree Height (Task 4)
    // ==========================================================
    printf("\n\n--- Task 4: Tree Height Calculation ---\n");
    int height = calculateHeight(root); ///< Calculate the tree's height
    printf("The calculated height of the tree is: %d\n", height);

    // ==========================================================
    // STEP 6: Evaluation & Truth Table (Task 5)
    // ==========================================================
    printf("\n\n--- Task 5: Evaluation & Truth Table ---\n");

    /// \brief Collect all unique variables (literals) from the tree.
    /// \details This is needed for both single evaluation and the full truth table.
    char **literals_list = NULL;
    int literal_count = collectUniqueLiterals(root, &literals_list);

    // --- Part 1: Single Evaluation from a user-provided file ---
    if (literal_count > 0)
    {
        printf("--- Single Evaluation From File ---\n");
        printf("The formula contains %d unique variables.\n", literal_count);
        printf("Please provide a file with truth assignments for each (e.g., 'x1 = T').\n\n");

        char filename_buffer[256];
        printf("Enter the path to your assignments file (or type 'skip' to cancel): ");

        if (scanf("%255s", filename_buffer) == 1)
        {
            if (strcmp(filename_buffer, "skip") != 0)
            {
                ///< Evaluate the tree using truth values from the specified file.
                evaluateFromFile(root, filename_buffer);
            }
            else
            {
                printf("Skipping single evaluation.\n");
            }
        }
        else
        {
            fprintf(stderr, "Failed to read filename. Skipping evaluation.\n");
            ///< Clear stdin in case of malformed input (e.g., "file name")
            while (getchar() != '\n');
        }
    }
    else
    {
        printf("No literals found. The expression is a constant.\n");
    }

    // --- Part 2: Generate Full Truth Table ---
    printf("\n--- Full Truth Table ---\n");

    if (literal_count > 0)
    {
        /**
         * \brief Print the full truth table for the formula.
         * \note The 'buffer' (original infix string) is used for the table header.
         */
        printTruthTable(root, literals_list, literal_count, buffer);
    }
    else
    {
        ///< If no literals, just evaluate the constant expression.
        bool result = evaluateTree(root, NULL);
        printf("The constant expression evaluates to: %s\n", result ? "True" : "False");
    }

    /// \brief Clean up the dynamically allocated list of literals.
    if (literals_list)
    {
        for (int i = 0; i < literal_count; i++)
        {
            free(literals_list[i]); ///< Free each literal string
        }
        free(literals_list); ///< Free the array of pointers
    }

    // ==========================================================
    // STEP 7: Convert to CNF (Task 6)
    // ==========================================================
    printf("\n\n--- Task 6: CNF Conversion ---\n");
    TreeNode *cnf_root = CNF_FORMULA(root); ///< Convert the original tree to CNF
    if (cnf_root)
    {
        printf("Final CNF formula: ");
        print_formula(cnf_root); ///< Print the resulting CNF formula
        printf("\n");
        ///< \note cnf_root is NOT freed here; it's needed for Task 7.
    }
    else
    {
        printf("CNF conversion failed.\n");
    }

    // ==========================================================
    // STEP 8: CNF Validity Check (Task 7)
    // ==========================================================

    if (cnf_root)
    {
        printf("\n--- Testing CNF Validity (Task 7) ---\n");
        int valid_count = 0;   ///< Counter for tautological clauses
        int invalid_count = 0; ///< Counter for non-tautological clauses

        ///< Check if the entire CNF formula is a tautology (all clauses are valid).
        bool is_tautology = checkCNFValidity(cnf_root, &valid_count, &invalid_count);

        printf("Analysis of the CNF formula:\n");
        printf("- Valid (Tautological) Clauses: %d\n", valid_count);
        printf("- Invalid Clauses: %d\n", invalid_count);
        printf("- Is the entire formula a tautology? %s\n", is_tautology ? "Yes" : "No");

        freeTree(cnf_root); ///< Now we can free the CNF tree
    }

    // ==========================================================
    // FINAL CLEANUP
    // ==========================================================
    printf("\n--- All tasks complete. Cleaning up. ---\n");

    free(buffer);   ///< Free the initial infix string read from the CNF
    freeTree(root); ///< Free the main parse tree from Task 2

    return 0; ///< Success
}