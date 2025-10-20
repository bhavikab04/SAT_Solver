#include <stdio.h>
#include <stdbool.h>
#include <stdlib.h>
#include <string.h>

// Include all task headers
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
    // --- Argument Check ---
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
    // STEP 1: CNF to Infix Conversion
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
        free(buffer);
        return 1;
    }

    printf("Generated Infix: %s\n", buffer);

    // ==========================================================
    // STEP 2 & 3: Infix to Prefix (Task 1) & Prefix to Tree (Task 2)
    // ==========================================================
    Stack *prefix_stack = task1_infixToPrefix(buffer);
    
    // Declare root here in the outer scope
    TreeNode *root = NULL; 

    if (prefix_stack)
    {
        printf("\n--- Task 1 Result (Prefix Stack) & Task 2 (Tree Build) ---\n");
        
        // prefixToTree consumes the stack
        root = prefixToTree(prefix_stack);

        if (root)
        {
            printTreeVertical(root);
        }
        else
        {
            printf("Failed to build the parse tree from the stack.\n");
        }
        
        // Note: prefix_stack is now empty and can be freed.
        // If prefixToTree fails, the stack might still have data,
        // so we free it *after* the if/else.
        freeStack(prefix_stack); 
    }
    else
    {
        printf("Failed to convert the infix expression to a stack.\n");
    }


    if (root == NULL)
    {
        printf("\nAborting further tasks because the parse tree could not be built.\n");
        free(buffer); // Free the buffer before exiting
        return 1;
    }

    // ==========================================================
    // STEP 4: Infix Reconstruction (Task 3)
    // ==========================================================
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

    // ==========================================================
    // STEP 5: Tree Height (Task 4)
    // ==========================================================
    printf("\n\n--- Task 4: Tree Height Calculation ---\n");
    int height = calculateHeight(root);
    printf("The calculated height of the tree is: %d\n", height);

    // ==========================================================
    // STEP 6: Evaluation & Truth Table (Task 5)
    // ==========================================================
    printf("\n\n--- Task 5: Evaluation & Truth Table ---\n");
    
    // This part finds all literals (like 'x1', 'x2')
    // and generates the full truth table.
    char **literals_list = NULL;
    int literal_count = collectUniqueLiterals(root, &literals_list);
    if (literal_count > 0)
    {
        // *** FIXED ***: Use 'buffer' as the label, not 'test_expression'
        printTruthTable(root, literals_list, literal_count, buffer);
        for (int i = 0; i < literal_count; i++)
        {
            free(literals_list[i]);
        }
        free(literals_list);
    }

    // ==========================================================
    // STEP 7: Convert to CNF (Task 6)
    // ==========================================================
    printf("\n\n--- Task 6: CNF Conversion ---\n");
    TreeNode *cnf_root = CNF_FORMULA(root);
    if (cnf_root)
    {
        printf("Final CNF formula: ");
        print_formula(cnf_root);
        printf("\n");
        // *** MOVED ***: Do not free cnf_root here, Task 7 needs it
    }
    else
    {
        printf("CNF conversion failed.\n");
    }

    // ==========================================================
    // STEP 8: CNF Validity Check (Task 7)
    // ==========================================================
    
    // *** FIXED ***: Check 'cnf_root' *after* it has been created
    if (cnf_root)
    {
        printf("\n--- Testing CNF Validity (Task 7) ---\n");
        int valid_count = 0;
        int invalid_count = 0;

        bool is_tautology = checkCNFValidity(cnf_root, &valid_count, &invalid_count);

        printf("Analysis of the CNF formula:\n");
        printf("- Valid (Tautological) Clauses: %d\n", valid_count);
        printf("- Invalid Clauses: %d\n", invalid_count);
        printf("- Is the entire formula a tautology? %s\n", is_tautology ? "Yes" : "No");

        // *** FIXED ***: Free cnf_root *after* you are done using it
        freeTree(cnf_root);
    }

    // ==========================================================
    // FINAL CLEANUP
    // ==========================================================
    printf("\n--- All tasks complete. Cleaning up. ---\n");
    
    // *** FIXED ***: Free the memory allocated at the start
    free(buffer);   // Free the infix string
    freeTree(root); // Free the main parse tree

    return 0;
}