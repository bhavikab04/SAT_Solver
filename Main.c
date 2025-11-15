/**
 * \file main.c
 * \brief Main driver for the logic formula processing pipeline.
 * \author [Team]
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
#include <time.h>
// Include all task headers
#include "convertingCNFtoInput.h" ///< For get_clause_count, convertCnfToInfix
#include "Task1.h"                ///< For Stack, read_line, task1_infixToPrefix
#include "Task2.h"                ///< For TreeNode, prefixToTree, printTreeVertical, freeTree
#include "Task3.h"                ///< For getExpLength, inOrderTraversal
#include "Task4.h"                ///< For find_height
#include "Task5.h"                ///< For evaluateFromFile, get_Unique_Literals, printTruthTable, evaluateTree
#include "Task6.h"                ///< For CNF_FORMULA, print_formula
#include "Task7.h"                ///< For checkCNFValidity


// --- Platform-Specific Memory Usage (Linux) ---
/**
 * \brief Gets current resident memory usage (RSS) in Kilobytes.
 * \note This function is platform-specific and **only works on Linux**.
 * \details It reads the /proc/self/status file to find the "VmRSS:" line.
 * \return long The current memory usage in KB, or -1 if unsupported/error.
 */
long get_current_memory_kb() {
    FILE* f = fopen("/proc/self/status", "r");
    if (f == NULL) {
        // This will happen on non-Linux systems (Windows, macOS)
        return -1;
    }
    char line[128];
    long vm_rss_kb = -1;
    while (fgets(line, 128, f) != NULL) {
        // VmRSS is the Resident Set Size: physical memory being used.
        if (strncmp(line, "VmRSS:", 6) == 0) {
            char* num_str = line + 6; // Skip "VmRSS:"
            while (*num_str == ' ' || *num_str == '\t') num_str++; // Skip whitespace
            vm_rss_kb = atol(num_str);
            break;
        }
    }
    fclose(f);
    return vm_rss_kb;
}
// --- End Memory Usage ---


// --- CSV Logging ---
/**
 * \brief Appends performance (time and memory) data to a CSV file.
 * \details If the file doesn't exist or is empty, it writes a header row first.
 * \param[in] filename The name of the CSV file to write to.
 * \param[in] m_start The initial memory usage (KB).
 * \param[in] m_end The final memory usage (KB).
 * \param[in] times Array of 8 double values for task times.
 * \param[in] total_time The total execution time.
 * \param[in] n The number of elements in the times array (should be 8).
 */
void log_to_csv(const char* filename, long m_start, long m_end, double times[], double total_time, int n) {
    FILE* f = fopen(filename, "a+"); // "a+" (append, create if not exists, read/write)
    if (f == NULL) {
        perror("Error opening CSV log file");
        return;
    }

    // Check if file is empty to write header
    fseek(f, 0, SEEK_END);
    long size = ftell(f);
    if (size == 0) {
        fprintf(f, "Initial Mem (KB),Final Mem (KB),Task 1 Time (s),Task 2 Time (s),Task 3 Time (s),Task 4 Time (s),Task 5 Time (s),Task 6 Time (s),Task 7 Time (s),Task 8 Time (s),Total Time (s)\n");
    }
    
    // Append the new data row
    fprintf(f, "%ld,%ld,", m_start, m_end);
    for (int i = 0; i < n; i++) {
        fprintf(f, "%lf,", times[i]);
    }
    fprintf(f, "%lf\n", total_time);

    fclose(f);
    printf("\nSuccessfully appended performance data to %s\n", filename);
}
// --- End CSV Logging ---


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
    // --- Performance Tracking Initialization ---
    // Time tracking
    clock_t t_start, t_step1, t_step2, t_step3, t_step4, t_step5, t_step6, t_step7, t_end;
    // Memory tracking (Linux-specific)
    long m_start, m_step1, m_step2, m_step3, m_step4, m_step5, m_step6, m_step7, m_end;
    bool memory_tracking_enabled = true;

    t_start = clock();
    m_start = get_current_memory_kb();
    if (m_start == -1) {
        memory_tracking_enabled = false;
        printf("[Warning] Memory tracking is unsupported on this OS. Memory usage will be 0.\n");
    }
    // --- End Initialization ---


    //    Argument Check 
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

    
    // STEP 1: CNF to Infix Conversion
    
    long num_clauses = get_clause_count(cnf_filename);
    if (num_clauses <= 0)
    {
        fprintf(stderr, "Error: Could not find 'p cnf' header in %s\n", cnf_filename);
        return 1;
    }

    FILE *temp_stream = tmpfile();
    if (temp_stream == NULL)
    {
        perror("Error creating temporary file");
        return 1;
    }
    convertCnfToInfix(cnf_filename, num_clauses, temp_stream);
    rewind(temp_stream);
    buffer = read_line(temp_stream);
    fclose(temp_stream); 

    if (buffer == NULL || strlen(buffer) == 0)
    {
        fprintf(stderr, "Error: CNF conversion produced no output.\n");
        free(buffer);
        return 1;
    }

    printf("Generated Infix: %s\n", buffer);
    t_step1 = clock();
    m_step1 = get_current_memory_kb();

    
    // STEP 2 & 3: Infix to Prefix (Task 1) & Prefix to Tree (Task 2)
    
    Stack *prefix_stack = task1_infixToPrefix(buffer);
    TreeNode *root = NULL; 

    if (prefix_stack)
    {
        printf("\n--- Task 1 Result (Prefix Stack) & Task 2 (Tree Build) ---\n");
        root = prefixToTree(prefix_stack); 
        if (root)
        {
            printTreeVertical(root); 
        }
        else
        {
            printf("Failed to build the parse tree from the stack.\n");
        }
        freeStack(prefix_stack);
    }
    else
    {
        printf("Failed to convert the infix expression to a stack.\n");
    }

    if (root == NULL)
    {
        printf("\nAborting further tasks because the parse tree could not be built.\n");
        free(buffer); 
        free(root);   
        return 1;
    }
    t_step2 = clock();
    m_step2 = get_current_memory_kb();
    
    // STEP 4: Infix Reconstruction (Task 3)
    
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
    t_step3 = clock();
    m_step3 = get_current_memory_kb();

    // 
    // STEP 5: Tree Height (Task 4)
    //
    printf("\n\n--- Task 4: Tree Height Calculation ---\n");
    int height = find_height(root); 
    printf("The calculated height of the tree is: %d\n", height);
    t_step4 = clock();
    m_step4 = get_current_memory_kb();

    // 
    // STEP 6: Evaluation & Truth Table (Task 5)
    // 
    printf("\n\n--- Task 5: Evaluation & Truth Table ---\n");

    char **literals_list = NULL;
    int literal_count = get_Unique_Literals(root, &literals_list);
 
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
            while (getchar() != '\n');
        }
    }
    else
    {
        printf("No literals found. The expression is a constant.\n");
    }

    
    printf("\n--- Full Truth Table ---\n");
    if (literal_count > 0)
    {
        printTruthTable(root, literals_list, literal_count, buffer);
    }
    else
    {
        bool result = evaluateTree(root, NULL);
        printf("The constant expression evaluates to: %s\n", result ? "True" : "False");
    }

    if (literals_list)
    {
        for (int i = 0; i < literal_count; i++)
        {
            free(literals_list[i]); 
        }
        free(literals_list); 
    }
    t_step5 = clock();
    m_step5 = get_current_memory_kb();
    
    // STEP 7: Convert to CNF (Task 6)
  
    printf("\n\n--- Task 6 : Manual Infix to CNF ---\n");
    printf("Enter a new infix formula to convert to CNF (or type 'skip'):\n");

    int c;
    while ((c = getchar()) != '\n' && c != EOF); // Clear stdin buffer

    printf("> ");
    char *user_infix_buffer = read_line(stdin); 

    if (user_infix_buffer == NULL || strlen(user_infix_buffer) == 0 || strcmp(user_infix_buffer, "skip") == 0)
    {
        printf("Skipping manual CNF conversion.\n");
    }
    else
    {
        printf("Your Infix: %s\n", user_infix_buffer);

        Stack *user_prefix_stack = task1_infixToPrefix(user_infix_buffer);
        TreeNode *user_tree_root = NULL;
        if (user_prefix_stack)
        {
            user_tree_root = prefixToTree(user_prefix_stack);
            freeStack(user_prefix_stack); 
        }
        else
        {
            printf("Failed to convert your infix string to prefix.\n");
        }

        if (user_tree_root)
        {
            printf("\n--- Converting your input tree to CNF ---\n");
            printf("Your Parse Tree:\n");
            printTreeVertical(user_tree_root);
            
            TreeNode *user_cnf_root = CNF_FORMULA(user_tree_root);
            if (user_cnf_root)
            {
                printf("\nFinal CNF for your formula: ");
                print_formula(user_cnf_root);
                printf("\n");
                freeTree(user_cnf_root); 
            }
            else
            {
                printf("CNF conversion failed for your input.\n");
            }
            freeTree(user_tree_root);
        }
        else
        {
            printf("Failed to build parse tree from your infix string.\n");
        }
    }
    free(user_infix_buffer); 


    t_step6 = clock();
    m_step6 = get_current_memory_kb();

    // STEP 8: CNF Validity Check (Task 7)
    if (root)
    {
        printf("\n--- Testing CNF Validity (Task 7) ---\n");
        int valid_count = 0;   
        int invalid_count = 0; 

        bool is_tautology = checkCNFValidity(root, &valid_count, &invalid_count);

        printf("Analysis of the CNF formula:\n");
        printf("- Valid (Tautological) Clauses: %d\n", valid_count);
        printf("- Invalid Clauses: %d\n", invalid_count);
        printf("- Is the entire formula a tautology? %s\n", is_tautology ? "Yes" : "No");

        freeTree(root); 
    }

    t_step7 = clock();
    m_step7 = get_current_memory_kb();
    
    // FINAL CLEANUP
    
    printf("\n--- All tasks complete. Cleaning up. ---\n");

    free(buffer);   
    // freeTree(root); // Already freed in Task 7
    t_end = clock();
    m_end = get_current_memory_kb();

    // --- Performance Calculation and Output ---
    
    // Store clock and memory snapshots
    clock_t store_time[] = {t_start, t_step1, t_step2, t_step3, t_step4, t_step5, t_step6, t_step7, t_end};
    // long store_mem[] = {m_start, m_step1, m_step2, m_step3, m_step4, m_step5, m_step6, m_step7, m_end}; // Not needed for CSV
    
    // Arrays to hold the *delta* (difference) for each step
    double times[8];
    // long mem[8]; // Not needed for CSV
    
    for(int i = 0; i < 8; i++){
        times[i] = ((double)(store_time[i+1] - store_time[i])) / CLOCKS_PER_SEC;
        
        // mem[i] = store_mem[i+1]; // Not needed for CSV
    }

    printf("\n--- Task Runtimes ---\n");
    printf("Step 1 (CNF to Infix):   %lf s\n", times[0]);
    printf("Step 2/3 (Infix->Tree):  %lf s\n", times[1]);
    printf("Step 4 (Tree to Infix):  %lf s\n", times[2]);
    printf("Step 5 (Tree Height):    %lf s\n", times[3]);
    printf("Step 6 (Evaluation):     %lf s\n", times[4]);
    printf("Step 7 (Manual CNF):     %lf s\n", times[5]);
    printf("Step 8 (CNF Validity):   %lf s\n", times[6]);
    printf("Cleanup:                 %lf s\n", times[7]);
    printf("----------------------------------\n");
    double total_time = ((double)(t_end - t_start) / CLOCKS_PER_SEC);
    printf("Total Time:              %lf s\n", total_time);

    if (memory_tracking_enabled) {
        // printf("\n--- Task Memory Usage (Peak KB at step end) ---\n"); // Not needed
        // ... (removed per-task memory print)
        printf("\n--- Memory Usage ---\n");
        printf("Initial Memory:          %ld KB\n", m_start);
        printf("Final Memory:            %ld KB\n", m_end);
    }

    // Log to CSV file
    log_to_csv("performance_log.csv", m_start, m_end, times, total_time, 8);

    return 0; ///< Success
}

