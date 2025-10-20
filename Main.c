#include <stdio.h>
#include <stdbool.h>
#include <stdlib.h>
#include <string.h>
#include "convertingCNFtoInput.h" // For CNF conversion
#include "Task1.h"                // For Stack, read_line, task1_infixToPrefix
#include "Task2.h"
#include "Task3.h"
#include "Task4.h"
#include "Task5.h"
#include "Task6.h"
#include "Task7.h"


int main(int argc, char *argv[])
{
    // We must add argc/argv to accept the CNF filename
    if (argc != 2)
    {
        fprintf(stderr, "Usage: %s <filename.cnf>\n", argv[0]);
        fprintf(stderr, "This will run the CNF->Infix->Prefix->... pipeline.\n");
        return 1;
    }
    const char *cnf_filename = argv[1];
    char *buffer = NULL; // This will hold the infix string

    printf("--- Main Test Function ---\n");
    printf("Processing CNF file: %s\n", cnf_filename);

    // ==========================================================
    // --- ADDED: CNF to Infix Conversion ---
    // This part generates the 'buffer' needed for Task 1
    // ==========================================================
    long num_clauses = get_clause_count(cnf_filename);
    if (num_clauses <= 0)
    {
        fprintf(stderr, "Error: Could not find 'p cnf' header in %s\n", cnf_filename);
        return 1;
    }

    FILE *temp_stream = tmpfile(); // Create a temporary in-memory file
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
    fclose(temp_stream); // Temp file is auto-deleted

    if (buffer == NULL || strlen(buffer) == 0)
    {
        fprintf(stderr, "Error: CNF conversion produced no output.\n");
        
        return 1;
    }

    printf("Generated Infix: %s\n", buffer);

    const char *test_expression = "~*p>qr";

    // end of changes that I made- Pujitha

    printf("Testing with expression: %s\n", test_expression);

    // Task 2:

    // ... input reading logic ...
    Stack *prefix_stack = task1_infixToPrefix(buffer);
    
    // Declare root here in the outer scope and initialize to NULL
    TreeNode *root = NULL; 

    if (prefix_stack)
    {
        printf("\n--- Task 1 Result ---\n");
        // ... (Code to print/handle the prefix stack) ...

        printf("\n--- Task 2 Execution ---\n");
        
        // Now, this is an assignment, not a new declaration.
        root = prefixToTree(prefix_stack);

        if (root)
        {
            printTreeVertical(root);
        }
        else
        {
            printf("Failed to build the parse tree from the stack.\n");
        }
    }
    else
    {
        printf("Failed to convert the prefix expression to a stack.\n");
    }


    if (root == NULL)
    {
        printf("\nAborting further tasks because the parse tree could not be built.\n");
        return 1;
    }
    // --- Task 3: Infix Expression Reconstruction ---
    printf("\n\n--- Task 3: Infix Expression Reconstruction ---\n");
    int bufferLength = getExpLength(root);
    char *infix = (char *)malloc(bufferLength + 1);
    if (infix)
    {
        int pos = 0;
        inOrderTraversal(root, infix, &pos);
        infix[pos] = '\0';
        printf("Reconstructed Infix: %s\n", infix);
        free(infix);
    }
    else
    {
        printf("Memory allocation failed for infix buffer string\n");
    }

    // --- Task 4: Calculate Tree Height ---
    printf("\n\n--- Task 4: Tree Height Calculation ---\n");
    int height = calculateHeight(root);
    printf("The calculated height of the tree is: %d\n", height);

        // --- Task 5: Evaluation & Truth Table ---
    printf("\n\n--- Task 5: Evaluation & Truth Table ---\n");

    // First, find out what variables we are dealing with for all parts of Task 5.
    char **literals_list = NULL;
    int literal_count = collectUniqueLiterals(root, &literals_list);

    // --- Part 1: Single Evaluation from a user-provided file ---
    if (literal_count > 0)
    {
        printf("--- Single Evaluation From File ---\n");
        // 1. Inform the user of the required inputs.
        printf("The formula contains %d unique variables.\n", literal_count);
        printf("Please provide a file with truth assignments for each (e.g., 'p = T').\n\n");

        // 2. Prompt the user for the filename.
        char filename_buffer[256];
        printf("Enter the path to your assignments file (or type 'skip' to cancel): ");

        // 3. Read the filename safely.
        if (scanf("%255s", filename_buffer) == 1)
        {
            // Allow the user to skip this step.
            if (strcmp(filename_buffer, "skip") != 0)
            {
                // 4. Call the efficient file evaluation function.
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
            // Clear the input buffer in case of bad input (e.g., "hello world")
            while (getchar() != '\n');
        }
    }
    else
    {
        // This case handles formulas without variables like "T + F"
        printf("No literals found. The expression is a constant.\n");
    }

    // --- Part 2: Generate Full Truth Table ---
    printf("\n--- Full Truth Table ---\n");

    // The printTruthTable function now contains its own safety check using MAX_TRUTH_TABLE_VARIABLES.
    if (literal_count > 0)
    {
        // The 'buffer' variable holds the original infix string for the header.
        printTruthTable(root, literals_list, literal_count, buffer);
    }
    else
    {
        // If no literals, just print the single evaluation result.
        // Passing NULL for assignments is safe for constant expressions.
        bool result = evaluateTree(root, NULL);
        printf("The constant expression evaluates to: %s\n", result ? "True" : "False");
    }

    // --- Cleanup for literals_list ---
    if (literals_list) {
        for (int i = 0; i < literal_count; i++)
        {
            free(literals_list[i]);
        }
        free(literals_list);
    }



    // --- Task 6: Convert to CNF ---
    printf("\n\n--- Task 6: CNF Conversion ---\n");
    TreeNode *cnf_root = CNF_FORMULA(root);
    if (cnf_root)
    {
        printf("Final CNF formula: ");
        print_formula(cnf_root);
        printf("\n");
    }
    else
    {
        printf("CNF conversion failed.\n");
    }

    // --- Task 7: CNF Validity Check (Independent of the tree) ---
    // printf("\n\n--- Task 7: CNF Validity Check ---\n");
    // const char *cnf_formula_valid = "p|q|~p & ~q|r|q";
    // In Main.c, after Task 6 creates cnf_root...

    if (cnf_root)
    {
        printf("\n--- Testing CNF Validity (Task 7) ---\n");
        int valid_count = 0;
        int invalid_count = 0;

        // Call the new function with the CNF tree from Task 6
        bool is_tautology = checkCNFValidity(cnf_root, &valid_count, &invalid_count);

        printf("Analysis of the CNF formula:\n");
        printf("- Valid (Tautological) Clauses: %d\n", valid_count);
        printf("- Invalid Clauses: %d\n", invalid_count);
        printf("- Is the entire formula a tautology? %s\n", is_tautology ? "Yes" : "No");

    }
    freeTree(cnf_root);
    free(buffer);
    return 0;
}
