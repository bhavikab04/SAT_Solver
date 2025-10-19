#include <stdio.h>
#include <stdbool.h>
#include <stdlib.h>
#include "convertingCNFtoInput.h" // For CNF conversion
#include "Task1.h"         // For Stack, read_line, task1_infixToPrefix
#include "Task2.h"
#include "Task3.h"
#include "Task4.h"
#include "Task5.h" 

#include "Task7.h" 

int main(int argc, char *argv[]) {
    // We must add argc/argv to accept the CNF filename
    if (argc != 2) {
        fprintf(stderr, "Usage: %s <filename.cnf>\n", argv[0]);
        fprintf(stderr, "This will run the CNF->Infix->Prefix->... pipeline.\n");
        return 1;
    }
    const char* cnf_filename = argv[1];
    char* buffer = NULL; // This will hold the infix string

    printf("--- Main Test Function ---\n");
    printf("Processing CNF file: %s\n", cnf_filename);
    
    // ==========================================================
    // --- ADDED: CNF to Infix Conversion ---
    // This part generates the 'buffer' needed for Task 1
    // ==========================================================
    long num_clauses = get_clause_count(cnf_filename);
    if (num_clauses <= 0) {
        fprintf(stderr, "Error: Could not find 'p cnf' header in %s\n", cnf_filename);
        return 1;
    }

    FILE* temp_stream = tmpfile(); // Create a temporary in-memory file
    if (temp_stream == NULL) {
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

    if (buffer == NULL || strlen(buffer) == 0) {
        fprintf(stderr, "Error: CNF conversion produced no output.\n");
        free(buffer);
        return 1;
    }

    printf("Generated Infix: %s\n", buffer);

    const char *test_expression = "~*p>qr";

//end of changes that I made- Pujitha



    printf("Testing with expression: %s\n", test_expression);

    //Task 2:

// ... input reading logic ...

    Stack* prefix_stack = task1_infixToPrefix(buffer);

    if (prefix_stack) {
        printf("\n--- Task 1 Result ---\n");
        // NOTE: The stack is printed by popping, which consumes it.
        // We must *replicate* the stack if we want to print AND use it for Task 2.
        // For simplicity, let's assume Task 1's main prints it first.
        
        // ... (Code to print prefix and free/re-create the stack, OR a simpler approach) ...

        printf("\n--- Task 2 Execution ---\n");
        // ** The prefixToTree function will consume (pop) the stack.**
        TreeNode* root = prefixToTree(prefix_stack); 

        if (root) {
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

    // --- Proceed with other tasks only if the tree was successfully built ---
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

    // --- Task 5: Evaluation and Truth Table ---
    printf("\n\n--- Task 5: Evaluation & Truth Table ---\n");
    TruthAssignment assignments[] = {
        {.literal = "p", .value = true},
        {.literal = "q", .value = false},
        {.literal = "r", .value = true}};
    int num_assignments = sizeof(assignments) / sizeof(assignments[0]);

    printf("With p=T, q=F, r=T, the formula evaluates to: %s\n",
           evaluateTree(root, assignments, num_assignments) ? "True" : "False");

    char **literals_list = NULL;
    int literal_count = collectUniqueLiterals(root, &literals_list);
    if (literal_count > 0)
    {
        printTruthTable(root, literals_list, literal_count, test_expression);
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
        freeTree(cnf_root);
    }
    else
    {
        printf("CNF conversion failed.\n");
    }

    // --- Task 7: CNF Validity Check (Independent of the tree) ---
    //printf("\n\n--- Task 7: CNF Validity Check ---\n");
    //const char *cnf_formula_valid = "p|q|~p & ~q|r|q";
    // In Main.c, after Task 6 creates cnf_root...

    if (cnf_root) {
        printf("\n--- Testing CNF Validity (Task 7) ---\n");
        int valid_count = 0;
        int invalid_count = 0;

        // Call the new function with the CNF tree from Task 6
        bool is_tautology = checkCNFValidity(cnf_root, &valid_count, &invalid_count);

        printf("Analysis of the CNF formula:\n");
        printf("- Valid (Tautological) Clauses: %d\n", valid_count);
        printf("- Invalid Clauses: %d\n", invalid_count);
        printf("- Is the entire formula a tautology? %s\n", is_tautology ? "Yes" : "No");

        // Don't forget to free the CNF tree
        freeTree(cnf_root);
    }

    return 0;
}
