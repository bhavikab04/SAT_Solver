\page analysis_page Project Algorithm Analysis

\section overview_sec Overall Review &  Summary

This document provides a detailed analysis of the multi-stage logic processing pipeline.

\par Introduction
This is a project that successfully implements a complex, multi-stage logic processing pipeline. The code is well-structured, highly modular, and demonstrates a strong command of C, data structures (stacks, trees, hash tables), and core algorithms (parsing, tree traversals, logical transformations).

The architectural design is clean: `main.c` acts as a central "driver" that passes a central data object (first a string, then a `TreeNode`) from one task module to the next. The Doxygen documentation is excellent and makes the code maintainable and easy to understand.

The analysis reveals a project of two halves:
1.  **Tasks 1-4 (Parsing & Analysis):** These are highly efficient, linear-time operations.
2.  **Tasks 5-6 (Evaluation & Transformation):** These run into **unavoidable** and **algorithmic** exponential bottlenecks, respectively, which correctly reflect the NP-hard nature of the problems being solved.

\subsection review_strengths Key Strengths 

* **Excellent Modularity:** Each task is self-contained in its own `.c` and `.h` file. This is a fantastic design choice. It makes the code easy to test, debug, and maintain.
* **Clean Data Flow:** The pipeline in `main.c` is logical and easy to follow: File \f$\to\f$ String \f$\to\f$ Stack \f$\to\f$ Tree \f$\to\f$ Analysis/Transformation. The `TreeNode` is the perfect central data structure for Tasks 2 through 7.
* **Strong Memory Management:** Diligent use of `free`, `freeStack`, and `freeTree` ensures no memory leaks. The `FINAL CLEANUP` section in `main.c` is exactly what's needed.
* **Good Algorithm Choices:**
    * The R-to-L Shunting-yard (Task 1) is a clever way to get to prefix notation.
    * The iterative `prefixToTree` (Task 2) using a helper stack is efficient and avoids deep recursion issues.
    * The use of **hash tables** (Task 5 & 7) provides \f$O(1)\f$ average-case lookup, which is far superior to any linear search.
* **Thorough Documentation:** The Doxygen comments are professional, clear, and correctly formatted.

\subsection review_bottlenecks Critical Performance & Memory Analysis 

Your pipeline has three major bottlenecks, each for a different reason.

1.  **The "Inherent" Exponential Bottleneck (Task 5) **
    * **What:** `printTruthTable` has a time complexity of **\f$O(2^V \cdot M \cdot k)\f$**.
    * **Why:** This is **unavoidable**. The *problem itself* (generating a full truth table) requires \f$2^V\f$ rows. For each row, you must evaluate the tree (\f$O(M \cdot k)\f$). This isn't a *flaw* in your code; it's a fundamental property of the task.

2.  **The "Algorithmic" Exponential Bottleneck (Task 6) **
    * **What:** `CNF_FORMULA` (specifically the `DISTR` function) has a time and space complexity of **\f$O(2^M)\f$**.
    * **Why:** Applying the distributive law `A + (B * C) -> (A + B) * (A + C)` **duplicates subtrees**. When this is done recursively, it can cause an exponential explosion in the *size of the tree*. This is the single greatest bottleneck in the entire project.

3.  **The "Implementation" Bottlenecks (Task 0 & Main) **
    * **`convertingCNFtoInput.c`:** The `build_clause_string` function is **\f$O(L^2 \cdot k)\f$** (quadratic) due to repeated string concatenation.
    * **`main.c`:** The `buffer` variable, which holds the *entire* infix string in memory, is a major memory hotspot for large input files.

---

\section key_sec Analysis Variable Key

This table defines the variables used in the complexity analysis.

| Variable | Description |
| :--- | :--- |
| \f$N\f$ | Length of the input **infix** string (for Task 1). |
| \f$M\f$ | Number of nodes/tokens in the **primary parse tree** (from Task 2). |
| \f$H\f$ | Height of the primary parse tree. |
| \f$V\f$ | Number of **unique variables** in the formula. |
| \f$k\f$ | Maximum string length of any single variable name (e.g., "x123"). |
| \f$F\f$ | Total size (in bytes) of the input `.cnf` file. |
| \f$C\f$ | Number of **clauses** in the CNF formula. |
| \f$L\f$ | Number of **literals** in a single clause (\f$L_{\text{max}}\f$ for max, \f$L_{\text{avg}}\f$ for average). |
| \f$S_{\text{total}}\f$ | Total length of the generated infix string (from Task 0). |
| \f$M_{\text{cnf}}\f$ | Total number of nodes in the *final generated CNF tree* (from Task 6). |

---

\section pipeline_sec Full Pipeline Analysis (main.c) 

This file orchestrates the entire process, so its complexity is the sum of all parts.

* **Output:** An `int` (0 for success) and all task results printed to the console.

\subsection main_time Time Complexity: \f$O(F + C \cdot L_{\text{max}}^2 \cdot k + S_{\text{total}} + 2^V \cdot M \cdot k + 2^M)\f$

This complex formula represents the sum of the bottlenecks.
* **\f$O(F + C \cdot L_{\text{max}}^2 \cdot k)\f$**: Time for `convertCnfToInfix` (Task 0). This can be large if a clause is very long.
* **\f$O(S_{\text{total}})\f$**: Time for `read_line` and `task1_infixToPrefix`.
* **\f$O(M \cdot k)\f$**: Time for tree building, infix reconstruction, height, etc. (Tasks 2, 3, 4).
* **\f$O(2^V \cdot M \cdot k)\f$**: The **first exponential bottleneck** from `printTruthTable` (Task 5).
* **\f$O(2^M)\f$**: The **second exponential bottleneck** from `CNF_FORMULA` (Task 6).

The pipeline's runtime is dominated by **Task 5 (Truth Table)** and **Task 6 (CNF Conversion)**.

\subsection main_space Space Complexity: \f$O(S_{\text{total}} + M \cdot k + V \cdot k + 2^M)\f$

The pipeline must hold several large data structures in memory.
* **\f$O(S_{\text{total}})\f$**: Space for `buffer` (the full infix string).
* **\f$O(M \cdot k)\f$**: Space for the primary parse tree `root`.
* **\f$O(V \cdot k)\f$**: Space for `literals_list` and the Task 5 hash table.
* **\f$O(2^M)\f$**: The **dominant space bottleneck** from `CNF_FORMULA` (Task 6). The final `cnf_root` tree can be exponentially large.

---

\section modules_sec Detailed Module Analysis

This section breaks down the analysis for each individual task module.

\subsection task0_sec Task 0: CNF to Infix (convertingCNFtoInput.c) 

This module is responsible for parsing the `.cnf` file.

* **`get_clause_count(const char* filename)`**
    * **Output:** A `long` clause count. (Side effect: stores variable count).
    * **Time:** \f$O(F)\f$, where \f$F\f$ is the file size. Reads line-by-line.
    * **Space:** \f$O(1)\f$ (uses a fixed-size `line_buffer`).

* **`build_clause_string(char* line)`**
    * **Output:** A dynamically allocated `char*` for a single clause.
    * **Time:** \f$O(L^2 \cdot k)\f$.
    * **Space:** \f$O(L \cdot k)\f$.

    \par  Implementation Bottleneck:
    This function has a hidden quadratic bottleneck. The `snprintf` call to build `new_clause` rereads the growing `clause_formula` string in each of the \f$L\f$ iterations. The total work is the sum of an arithmetic progression: \f$O(k \cdot (1 + 2 + ... + L)) = O(k \cdot L^2)\f$.

* **`convertCnfToInfix(...)`**
    * **Output:** Writes the full infix string to the `out_stream` (side effect).
    * **Time:** \f$O(F + C \cdot L_{\text{max}}^2 \cdot k)\f$. Dominated by reading the file (\f$O(F)\f$) and the \f$C\f$ calls to the quadratic `build_clause_string`.
    * **Space:** \f$O(L_{\text{max}} \cdot k)\f$. This is very memory-efficient as it *streams* the output, only holding the largest single clause string in memory.

\subsection task1_sec Task 1: Infix to Prefix (task1.c)

Implements a modified (right-to-left) Shunting-yard algorithm.

* **`task1_infixToPrefix(const char *infix)`**
    * **Output:** A `Stack*` containing tokens in prefix order.
    * **Time:** \f$O(N)\f$. The main `for` loop iterates \f$N\f$ times. All inner loops (operand parsing, operator stack pops) have an **amortized cost of \f$O(1)\f$**, as each character and token is processed a constant number of times. Total work is \f$O(N)\f$.
    * **Space:** \f$O(N)\f$. Both `op_stack` and `final_stack` can grow to \f$O(M)\f$ (or \f$O(N)\f$). The total storage for all `strdup`-ed token strings is also \f$O(N)\f$.

\par Helper Functions (task1.c)
| Function | Time Complexity | Space Complexity | Notes |
| :--- | :--- | :--- | :--- |
| `read_line` | \f$O(L)\f$ | \f$O(L)\f$ | \f$L\f$ is line length. Amortized \f$O(L)\f$ for `realloc`. |
| `push` | \f$O(k)\f$ | \f$O(k)\f$ | Dominated by `strdup` of item with length \f$k\f$. |
| `pop` | \f$O(1)\f$ | \f$O(1)\f$ | Returns a pointer. |
| `freeStack` | \f$O(M)\f$ | \f$O(1)\f$ | Calls `free` for all \f$M\f$ items in the stack. |

\subsection task2_sec Task 2: Prefix to Tree (Task2.c)

Implements an iterative algorithm to build a parse tree from a prefix stack.

* **`prefixToTree(Stack *prefix_stack)`**
    * **Output:** A `TreeNode*` which is the root of the parse tree.
    * **Time:** \f$O(M)\f$. The main `while` loop processes each of the \f$M\f$ tokens exactly once. All stack operations (`push`, `pop`, `peek`) are \f$O(1)\f$. The most expensive operation, `makeNode`, is called \f$M\f$ times.
    * A more precise time is \f$O(M + S)\f$ where \f$S\f$ is the sum of all token lengths (from `strdup`), but this simplifies to \f$O(M)\f$ (or \f$O(M \cdot k)\f$).
    * **Auxiliary Space:** \f$O(H)\f$. This is the space *not* including the output tree. The internal `node_stack` stores parent nodes, and its maximum size is equal to the height of the tree, \f$H\f$.

\par Helper Functions (Task2.c)
| Function | Time Complexity | Space Complexity | Notes |
| :--- | :--- | :--- | :--- |
| `freeTree` | \f$O(M)\f$ | \f$O(H)\f$ | Classic post-order traversal. Space is from recursion stack. |
| `printTree...` | \f$O(M \cdot H)\f$ | \f$O(H)\f$ | Visits \f$M\f$ nodes. Work at each node is \f$O(H)\f$ to copy prefix string. |

\subsection task3_sec Task 3: Tree to Infix (Task3.c)

Reconstructs the fully-parenthesized infix string from the parse tree.

* **`getExpLength(TreeNode *root)`**
    * **Output:** An `int` for the total required string length.
    * **Time:** \f$O(M)\f$ (or \f$O(M \cdot k)\f$). Visits every node once. Work per node is \f$O(1)\f$ or \f$O(k)\f$ (for `strlen` on atoms).
    * **Space:** \f$O(H)\f$ (for the recursive call stack).

* **`inOrderTraversal(TreeNode *root, ...)`**
    * **Output:** Populates the pre-allocated `bufferStr` (side effect).
    * **Time:** \f$O(M)\f$ (or \f$O(M \cdot k)\f$). Visits every node once. Work per node is dominated by `memcpy` of the token.
    * **Space:** \f$O(H)\f$ (for the recursive call stack).

\subsection task4_sec Task 4: Tree Height (Task4.c)

Calculates the height of the parse tree.

* **`find_height(TreeNode *root)`**
    * **Output:** An `int` representing the tree's height.
    * **Time:** \f$O(M)\f$. This is the textbook post-order traversal for height, visiting every node once.
    * **Space:** \f$O(H)\f$ (for the recursive call stack).

\subsection task5_sec Task 5: Evaluation & Truth Table (Task5.c) 

Implements formula evaluation and truth table generation.

* **Hash Table (`HT_Insert` / `HT_Get`)**
    * **Time:** \f$O(k)\f$ (Average). Dominated by the `djb2` hash function. Worst case is \f$O(V \cdot k)\f$ on collision.
    * **Space:** \f$O(V \cdot k)\f$ (to store \f$V\f$ variables of average length \f$k\f$).

* **`evaluateTree(TreeNode *root, ...)`**
    * **Output:** A `bool` truth value.
    * **Time:** \f$O(M \cdot k)\f$. Visits \f$M\f$ nodes. At leaf nodes, performs an \f$O(k)\f$ hash table lookup.
    * **Space:** \f$O(H)\f$ (for the recursive call stack).

* **`get_Unique_Literals(TreeNode *root, ...)`**
    * **Output:** An `int` \f$V\f$ and a `char**` list of unique variables (side effect).
    * **Time:** \f$O(M \cdot k)\f$. Traverses the tree (\f$O(M)\f$), calling `HT_Insert` (\f$O(k)\f$) at each atom.
    * **Space:** \f$O(V \cdot k + H)\f$. Requires a hash table (\f$O(V \cdot k)\f$) and recursion stack (\f$O(H)\f$).

* **`printTruthTable / Assign_value(...)`**
    * **Output:** Prints the formatted truth table to `stdout`.
    * **Time:** \f$O(2^V \cdot M \cdot k)\f$.

    \par ⏱️ Inherent Exponential Bottleneck:
    This complexity is **unavoidable**. The algorithm must generate \f$2^V\f$ rows. For **each** row, it must call `evaluateTree`, which takes \f$O(M \cdot k)\f$ time. The total is \f$O(2^V \cdot (V \cdot k + M \cdot k))\f$, which simplifies to \f$O(2^V \cdot M \cdot k)\f$.

    * **Space:** \f$O(V \cdot k + H)\f$. Dominated by the `current_assignments` hash table (\f$O(V \cdot k)\f$) and the call stack of `evaluateTree` (\f$O(H)\f$).

\subsection task6_sec Task 6: CNF Conversion (Task6.c)

Implements the three-stage (IMPL_FREE, NNF, CNF) conversion.

* **`IMPL_FREE(TreeNode *phi)`**
    * **Output:** A new tree with no `>` operators.
    * **Time:** \f$O(M)\f$. Traverses the tree and builds a new one. Work is proportional to the input tree size.
    * **Space:** \f$O(H)\f$ (for recursion stack).

* **`NNF(TreeNode *phi)`**
    * **Output:** A new tree in Negation Normal Form.
    * **Time:** \f$O(M)\f$. Traverses the tree, applying De Morgan's laws. Work is proportional to the *output* NNF tree size, which is linearly related to \f$M\f$.
    * **Space:** \f$O(H)\f$ (for recursion stack).

* **`DISTR(TreeNode *eta1, TreeNode *eta2)`**
    * **Output:** A new tree with ORs distributed over ANDs.
    * **Time:** \f$O(2^M)\f$ (Exponential).

    \par  Algorithmic Exponential Bottleneck:
    This is the source of the exponential blow-up. The rule `A + (B * C) -> (A + B) * (A + C)` **duplicates** the `A` subtree. When applied recursively (e.g., `(a*b) + (c*d)`), it causes an exponential explosion in the *size of the output tree*.

    * **Space:** \f$O(2^M)\f$ (Exponential). The *output tree itself* requires exponential space.

* **`CNF_FORMULA(TreeNode *phi)`**
    * **Output:** The final CNF tree.
    * **Time:** \f$O(2^M)\f$. The pipeline is `IMPL_FREE` (\f$O(M)\f$) \f$\to\f$ `NNF` (\f$O(M)\f$) \f$\to\f$ `CNF`. The `CNF` step is dominated by its call to `DISTR`.
    * **Space:** \f$O(2^M)\f$. The final `cnf_root` tree itself requires exponential space.

\subsection task7_sec Task 7: Tautology Check (Task7.c) 

Checks if a CNF formula is a tautology by checking each clause.

* **`find_Literals(...)`**
    * **Output:** Modifies `hash_table` and `is_tautology` flag (side effect).
    * **Time:** \f$O(L \cdot k)\f$ (Average). Traverses the \f$O(L)\f$ nodes of a clause. At each leaf, performs an \f$O(k)\f$ hash table operation.
    * **Space:** \f$O(L \cdot k)\f$. The hash table stores \f$O(L)\f$ literals (\f$O(L \cdot k)\f$) and the recursion stack can go \f$O(L)\f$ deep.

* **`isClauseTautology(TreeNode* clause_root)`**
    * **Output:** A `bool` indicating if the clause contains `x` and `~x`.
    * **Time:** \f$O(L \cdot k)\f$ (Average). Dominated by calling `find_Literals` and freeing the hash table.
    * **Space:** \f$O(L \cdot k)\f$ (to create the temporary hash table).

* **`checkCNFValidity(TreeNode* cnf_root, ...)`**
    * **Output:** A `bool` indicating if the *entire* CNF formula is a tautology.
    * **Time:** \f$O(M_{\text{cnf}} \cdot k)\f$ (Average). The function traverses the `*` (AND) spine of the CNF tree, visiting \f$C\f$ clauses. For each clause, it calls `isClauseTautology` (\f$O(L_{\text{avg}} \cdot k)\f$).
    * The total time is \f$O(C \cdot L_{\text{avg}} \cdot k)\f$, which is proportional to the total number of nodes in the CNF tree.
    * **Space:** \f$O(\max(C, L_{\text{max}} \cdot k))\f$. The max space is the *greater* of the recursion stack for the `*` spine (\f$O(C)\f$) and the temporary hash table space for the largest clause (\f$O(L_{\text{max}} \cdot k)\f$).