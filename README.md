# SAT_Solver

A project by:

S Dharshana (2023B5A71208H)
Bhavika Baburaj (2023B5A71210H)
Vedika Nirmal Kumar Singh (2023B4A71359H)
Sri Pujitha Konjeti (2023B1A71384H)

## Overall Review and Summary

This document provides a detailed analysis of the project's multi-stage logic processing pipeline.

### Introduction

**A Project By:**
S Dharshana (2023B5A71208H),
Bhavika Baburaj (2023B5A71210H),
Vedika Nirmal Kumar Singh (2023B4A71359H),
Sri Pujitha Konjeti (2023B1A71384H)

This project implements a complex, multi-stage logic processing pipeline. The code is well-structured,  modular, and shows a strong command of C and core computer science concepts, including data structures (stacks, trees, hash tables) and algorithms (parsing, tree traversals, logical transformations).

The architectural design is clean: `main.c` acts as a central "driver," passing a central data object (first a string, then a `TreeNode`) from one task module to the next. The Doxygen documentation makes the code maintainable and easy to understand.


The analysis reveals a project of two halves:
1.  **Tasks 1-4 (Parsing and Analysis):** These have no bottlenecks as such
2.  **Tasks 5-6 (Evaluation and Transformation):** These run into **unavoidable** and **algorithmic** exponential bottlenecks, respectively, which correctly reflect the NP-hard nature of the problems being solved.

### Key Strengths

* **Modularity:** Each task is self-contained in its own `.c` and `.h` file. This makes the code easy to test, debug, and maintain. It also helped us code the project in a better way and divide tasks.
* **Data Flow:** The pipeline in `main.c` is logical and easy to follow: File \f$\to\f$ String \f$\to\f$ Stack \f$\to\f$ Tree \f$\to\f$ Analysis/Transformation. The `TreeNode` is the central data structure for Tasks 2 through 7.
* **Memory Management:** We used  `free`, `freeStack`, and `freeTree` ensures no memory leaks. There's also a  `FINAL CLEANUP` section in `main.c` .
* **Our Algorithm Choices:**
    * The Right to left  Shunting-yard in Task 1 was our algorithm of choice to get to prefix notation.
    * The iterative `prefixToTree` in task 2 using a helper stack was found to be efficient and avoided potential deep recursion issues.
    * The use of **hash tables** in task 5 and task 7 provided \f$O(1)\f$ average-case lookup, which is better than any linear search.
* **Thorough Documentation:** We have tried to make the Doxygen comments as professional, clear, and correctly formatted as we could

### Critical Performance and Memory Analysis

Our pipeline has three major bottlenecks, each for a different reason.

1.  **The Inherent Exponential Bottleneck in task 5 **
    * **What:** `printTruthTable` has a time complexity of **\f$O(2^V \cdot M \cdot k)\f$**.
    * **Why:** This is **unavoidable**. The question/task itself, which isgenerating a full truth table demands \f$2^V\f$ rows. For each row, the tree must be evaluated (\f$O(M \cdot k)\f$). This isn't technically a flaw in the code; but rather a fundamental necessity of the task.

2.  **The Algorithmic Exponential Bottleneck in Task 6 **
    * **What:** `CNF_FORMULA` (in specific, the `DISTR` function) has a time and space complexity of **\f$O(2^M)\f$**.
    * **Why:** Applying the distributive law `A + (B * C) -> (A + B) * (A + C)` **duplicates subtrees**. When applied recursively, this can cause an exponential explosion in the *size of the tree*. This is the single biggest performance bottleneck in the entire project as per our analysis.

3.  **The Implementation Bottlenecks in Task 0 and the main function **
    * **`convertingCNFtoInput.c`:** The `build_clause_string` function is **\f$O(L^2 \cdot k)\f$** (quadratic) due to repeated string concatenation inside a loop.
    * **`main.c`:** The `buffer` variable, which holds the *entire* infix string in memory, is a major memory hotspot for large input files.

---

### Analysis Variable Key

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

### Full Pipeline Analysis (main.c)

This file orchestrates the entire process, so its complexity is effectively the sum of its parts.

* **Output:** An `int` (0 for success) and all task results printed to the console.

#### Time Complexity

This complex formula represents the sum of all parts, but it's dominated by the bottlenecks.
* **\f$O(F + C \cdot L_{\text{max}}^2 \cdot k)\f$**: Time for `convertCnfToInfix` (Task 0). This can be large if any single clause is too long.
* **\f$O(S_{\text{total}})\f$**: Time for `read_line` and `task1_infixToPrefix`.
* **\f$O(M \cdot k)\f$**: Time for tree building, infix reconstruction, height, etc. (Tasks 2, 3, 4).
* **\f$O(2^V \cdot M \cdot k)\f$**: The **first exponential bottleneck** from `printTruthTable` (Task 5).
* **\f$O(2^M)\f$**: The **second exponential bottleneck** from `CNF_FORMULA` (Task 6).

The pipeline's runtime is mainly dominated by **Task 5 (Truth Table)** and **Task 6 (CNF Conversion)**.

#### Space Complexity

The pipeline's memory footprint is determined by several large data structures held in memory at the same time.
* **\f$O(S_{\text{total}})\f$**: Space for `buffer` (the full infix string).
* **\f$O(M \cdot k)\f$**: Space for the primary parse tree `root`.
* **\f$O(V \cdot k)\f$**: Space for `literals_list` and the Task 5 hash table.
* **\f$O(2^M)\f$**: The **dominant space bottleneck** from `CNF_FORMULA` (Task 6). The final `cnf_root` tree can be exponentially large.

---

### Detailed Module Analysis

This section breaks down the analysis for each individual task module.

#### Task 0: CNF to Infix (convertingCNFtoInput.c)

This module parses the `.cnf` file and converts it into a single, long infix string.

* **`get_clause_count(const char* filename)`**
    * **Output:** A `long` clause count. (Side effect: stores variable count).
    * **Time:** \f$O(F)\f$, where \f$F\f$ is the file size. Reads line-by-line.
    * **Space:** \f$O(1)\f$ (aside from a fixed-size `line_buffer`).

* **`build_clause_string(char* line)`**
    * **Output:** A dynamically allocated `char*` for a single clause.
    * **Time:** \f$O(L^2 \cdot k)\f$.
    * **Space:** \f$O(L \cdot k)\f$.

    Implementation Bottleneck:
    This function contains a subtle quadratic bottleneck. The `snprintf` call to build `new_clause` has to re-read the growing `clause_formula` string in each of the \f$L\f$ iterations. The total work is the sum of an arithmetic progression: \f$O(k \cdot (1 + 2 + ... + L)) = O(k \cdot L^2)\f$.

* **`convertCnfToInfix(...)`**
    * **Output:** Writes the full infix string to the `out_stream` (side effect).
    * **Time:** \f$O(F + C \cdot L_{\text{max}}^2 \cdot k)\f$. Dominated by reading the file (\f$O(F)\f$) and the \f$C\f$ calls to the quadratic `build_clause_string`.
    * **Space:** \f$O(L_{\text{max}} \cdot k)\f$. This is very memory-efficient as it *streams* the output, only holding the largest single clause string in memory at a time.

#### Task 1: Infix to Prefix (task1.c)

Implements a modified (right-to-left) Shunting-yard algorithm.

* **`task1_infixToPrefix(const char *infix)`**
    * **Output:** A `Stack*` containing tokens in prefix order.
    * **Time:** \f$O(N)\f$. The main `for` loop iterates \f$N\f$ times. All inner loops (operand parsing, operator stack pops) have an **amortized cost of \f$O(1)\f$**, as each character and token is processed a constant number of times. Total work is \f$O(N)\f$.
    * **Space:** \f$O(N)\f$. Both `op_stack` and `final_stack` can grow to \f$O(M)\f$ (or \f$O(N)\f$). The total storage for all `strdup`-ed token strings is also \f$O(N)\f$.

Helper Functions (task1.c)
| Function | Time Complexity | Space Complexity | Notes |
| :--- | :--- | :--- | :--- |
| `read_line` | \f$O(L)\f$ | \f$O(L)\f$ | \f$L\f$ is line length. Amortized \f$O(L)\f$ for `realloc`. |
| `push` | \f$O(k)\f$ | \f$O(k)\f$ | Dominated by `strdup` of item with length \f$k\f$. |
| `pop` | \f$O(1)\f$ | \f$O(1)\f$ | Returns a pointer. |
| `freeStack` | \f$O(M)\f$ | \f$O(1)\f$ | Calls `free` for all \f$M\f$ items in the stack. |

#### Task 2: Prefix to Tree (Task2.c)

Implements an iterative algorithm to build a parse tree from a prefix stack.

* **`prefixToTree(Stack *prefix_stack)`**
    * **Output:** A `TreeNode*` which is the root of the parse tree.
    * **Time:** \f$O(M)\f$. The main `while` loop processes each of the \f$M\f$ tokens exactly once. All stack operations (`push`, `pop`, `peek`) are \f$O(1)\f$. The `makeNode` operation is called \f$M\f$ times.
    * A more precise time is \f$O(M \cdot k)\f$ to account for \f$M\f$ `strdup` calls of average length \f$k\f$.
    * **Auxiliary Space:** \f$O(H)\f$. This is the space *not* including the output tree. The internal `node_stack` stores parent nodes, and its maximum size is equal to the height of the tree, \f$H\f$.

Helper Functions (Task2.c)
| Function | Time Complexity | Space Complexity | Notes |
| :--- | :--- | :--- | :--- |
| `freeTree` | \f$O(M)\f$ | \f$O(H)\f$ | Classic post-order traversal. Space is from recursion stack. |
| `printTree...` | \f$O(M \cdot H)\f$ | \f$O(H)\f$ | Visits \f$M\f$ nodes. Work at each node is \f$O(H)\f$ to copy prefix string. |

#### Task 3: Tree to Infix (Task3.c)

Reconstructs the fully-parenthesized infix string from the parse tree.

* **`getExpLength(TreeNode *root)`**
    * **Output:** An `int` for the total required string length.
    * **Time:** \f$O(M \cdot k)\f$. Visits every node once. Work per node is \f$O(k)\f$ (for `strlen` on atoms) or \f$O(1)\f$ for operators.
    * **Space:** \f$O(H)\f$ (for the recursive call stack).

* **`inOrderTraversal(TreeNode *root, ...)`**
    * **Output:** Populates the pre-allocated `bufferStr` (side effect).
    * **Time:** \f$O(M \cdot k)\f$. Visits every node once. Work per node is dominated by `memcpy` (or `strcpy`) of the token.
    * **Space:** \f$O(H)\f$ (for the recursive call stack).

#### Task 4: Tree Height (Task4.c)

Calculates the height of the parse tree.

* **`find_height(TreeNode *root)`**
    * **Output:** An `int` representing the tree's height.
    * **Time:** \f$O(M)\f$. This is the textbook post-order traversal for height, visiting every node exactly once.
    * **Space:** \f$O(H)\f$ (for the recursive call stack).

#### Task 5: Evaluation & Truth Table (Task5.c)

Implements formula evaluation and truth table generation.

* **Hash Table (`HT_Insert` / `HT_Get`)**
    * **Time:** \f$O(k)\f$ (Average). Dominated by the `djb2` hash function, which runs in time proportional to the key length \f$k\f$. Worst case is \f$O(V \cdot k)\f$ on collision.
    * **Space:** \f$O(V \cdot k)\f$ (to store \f$V\f$ variables of average length \f$k\f$).

* **`evaluateTree(TreeNode *root, ...)`**
    * **Output:** A `bool` truth value.
    * **Time:** \f$O(M \cdot k)\f$. Visits \f$M\f$ nodes. At leaf nodes, performs an \f$O(k)\f$ hash table lookup.
    * **Space:** \f$O(H)\f$ (for the recursive call stack).

* **`get_Unique_Literals(TreeNode *root, ...)`**
    * **Output:** An `int` \f$V\f$ and a `char**` list of unique variables (side effect).
    * **Time:** \f$O(M \cdot k)\f$. Traverses the tree (\f$O(M)\f$), calling `HT_Insert` (\f$O(k)\f$ on average) at each atom.
    * **Space:** \f$O(V \cdot k + H)\f$. Requires a hash table (\f$O(V \cdot k)\f$) and recursion stack (\f$O(H)\f$).

* **`printTruthTable / Assign_value(...)`**
    * **Output:** Prints the formatted truth table to `stdout`.
    * **Time:** \f$O(2^V \cdot M \cdot k)\f$.

    Inherent Exponential Bottleneck:
    This complexity is **unavoidable**. The algorithm must generate \f$2^V\f$ rows. For **each** row, it must call `evaluateTree`, which takes \f$O(M \cdot k)\f$ time. The total is \f$O(2^V \cdot (V \cdot k + M \cdot k))\f$ (for assignment + evaluation), which simplifies to \f$O(2^V \cdot M \cdot k)\f$ (since \f$M \ge V\f$).

    * **Space:** \f$O(V \cdot k + H)\f$. Dominated by the `current_assignments` hash table (\f$O(V \cdot k)\f$) and the call stack of `evaluateTree` (\f$O(H)\f$).

#### Task 6: CNF Conversion (Task6.c)

Implements the three-stage (IMPL_FREE, NNF, CNF) conversion.

* **`IMPL_FREE(TreeNode *phi)`**
    * **Output:** A new tree with no `>` operators.
    * **Time:** \f$O(M)\f$. Traverses the tree and builds a new one. Work is proportional to the input tree size.
    * **Space:** \f$O(M)\f$. A new tree of size \f$O(M)\f$ is created. (Auxiliary space for recursion is \f$O(H)\f$).

* **`NNF(TreeNode *phi)`**
    * **Output:** A new tree in Negation Normal Form.
    * **Time:** \f$O(M)\f$. Traverses the tree, applying De Morgan's laws. Work is proportional to the *output* NNF tree size, which is linearly related to \f$M\f$.
    * **Space:** \f$O(M)\f$. A new tree of size \f$O(M)\f$ is created. (Auxiliary space for recursion is \f$O(H)\f$).

* **`DISTR(TreeNode *eta1, TreeNode *eta2)`**
    * **Output:** A new tree with ORs distributed over ANDs.
    * **Time:** \f$O(2^M)\f$ (Exponential).

    \par Algorithmic Exponential Bottleneck:
    This function is the source of the exponential blow-up. The rule `A + (B * C) -> (A + B) * (A + C)` **duplicates** the `A` subtree. When applied recursively (e.g., `(a*b) + (c*d)`), it causes an exponential explosion in the *size of the output tree*.

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
    * **Space:** \f$O(L \cdot k)\f$. The hash table stores \f$O(L)\f$ literals (\f$O(L \cdot k)\f$) and the recursion stack can go \f$O(L)\f$ deep (for a degenerate clause tree).

* **`isClauseTautology(TreeNode* clause_root)`**
    * **Output:** A `bool` indicating if the clause contains `x` and `~x`.
    * **Time:** \f$O(L \cdot k)\f$ (Average). Dominated by calling `find_Literals` and freeing the hash table.
    * **Space:** \f$O(L \cdot k)\f$ (to create the temporary hash table).

* **`checkCNFValidity(TreeNode* cnf_root, ...)`**
    * **Output:** A `bool` indicating if the *entire* CNF formula is a tautology.
    * **Time:** \f$O(M_{\text{cnf}} \cdot k)\f$ (Average). The function traverses the `*` (AND) spine of the CNF tree, visiting \f$C\f$ clauses. For each clause, it calls `isClauseTautology` (\f$O(L_{\text{avg}} \cdot k)\f$).
    * The total time is \f$O(C \cdot L_{\text{avg}} \cdot k)\f$, which is proportional to the total number of nodes in the CNF tree, \f$M_{\text{cnf}}\f$.
    * **Space:** \f$O(\max(H_{\text{cnf}}, L_{\text{max}} \cdot k))\f$. The max space is the *greater* of the recursion stack for the CNF tree (\f$O(H_{\text{cnf}})\f$) and the temporary hash table space for the largest clause (\f$O(L_{\text{max}} \cdot k)\f$).


\section example_trace_sec Example Pipeline Trace (example.cnf)

This section traces the data flow of the pipeline using a concrete example file, `example.cnf`.

\subsection example_input_sec Input File (example.cnf)

The pipeline begins when Task 0 is asked to parse the following `.cnf` file:

\code
c This is an example CNF file with 8 variables and 5 clauses.
c It is a satisfiable formula.
p cnf 8 5
1 -2 3 0
-1 4 5 0
2 -6 0
-3 7 -8 0
5 6 8 0
\endcode

\subsection example_task0_sec Task 0: CNF to Infix

`convertCnfToInfix` reads this file. It ignores comment (`c`) lines, parses the problem (`p`) line for metadata, and then processes the 5 clause lines. Each clause is built into an OR-separated string, and all 5 clauses are joined by the AND operator (`*`).

\par
Assuming the implementation converts number `1` to variable `v1`, `-2` to `~v2`, etc., the resulting infix string written to the output stream (and read into `main.c`'s `buffer`) is:

\verbatim
((v1 + ~v2 + v3) * (~v1 + v4 + v5) * (v2 + ~v6) * (~v3 + v7 + ~v8) * (v5 + v6 + v8))
\endverbatim

\par
This string becomes the \f$N\f$ (or \f$S_{\text{total}}\f$) length input for the next stage.

\subsection example_task1_sec Task 1: Infix to Prefix

`task1_infixToPrefix` consumes this large infix string. It processes the tokens from right-to-left (due to the modified Shunting-yard) to produce a prefix stack. The logical (top-to-bottom) order of the `final_stack` returned to `main.c` is:

\verbatim
*
*
*
*
+
+
v1
~v2
v3
+
+
~v1
v4
v5
+
v2
~v6
+
+
~v3
v7
~v8
+
+
v5
v6
v8
\endverbatim

\subsection example_task2_sec Task 2: Prefix to Tree

`prefixToTree` pops tokens from this prefix stack to build the parse tree. The resulting `root` `TreeNode` is a right-deep tree of 4 AND (`*`) operators.

\par
The leaves of this main "AND-spine" are the 5 clause subtrees. Each clause subtree is, in turn, a right-deep tree of OR (`+`) operators.

A visual representation of the tree structure (generated by Graphviz):

\dot
digraph ParseTree {
    rankdir=TB; /* Top-to-Bottom layout */
    node [shape=record, fontname=Arial, fontsize=10];
    edge [fontsize=10];

    /* Define the 4 AND nodes of the spine */
    n_root [label="<f0> | <f1> * | <f2>"];
    n2     [label="<f0> | <f1> * | <f2>"];
    n3     [label="<f0> | <f1> * | <f2>"];
    n4     [label="<f0> | <f1> * | <f2>"];

    /* Define the 5 clause leaf nodes */
    c1 [shape=ellipse, label="Clause 1\n(v1 + ~v2 + v3)"];
    c2 [shape=ellipse, label="Clause 2\n(~v1 + v4 + v5)"];
    c3 [shape=ellipse, label="Clause 3\n(v2 + ~v6)"];
    c4 [shape=ellipse, label="Clause 4\n(~v3 + v7 + ~v8)"];
    c5 [shape=ellipse, label="Clause 5\n(v5 + v6 + v8)"];

    /* Link the spine and clauses */
    n_root:f0 -> c1 [label=" left"];
    n_root:f2 -> n2 [label=" right"];
    
    n2:f0 -> c2 [label=" left"];
    n2:f2 -> n3 [label=" right"];
    
    n3:f0 -> c3 [label=" left"];
    n3:f2 -> n4 [label=" right"];
    
    n4:f0 -> c4 [label=" left"];
    n4:f2 -> c5 [label=" right"];
}
\enddot


\subsection example_tasks3_7_sec Subsequent Analysis Tasks

This `root` tree (which becomes the central data structure) is then passed to the remaining tasks.

* **Task 3 (Tree to Infix):** Reconstructs the original, fully-parenthesized infix string shown in the Task 0 example.

* **Task 4 (Tree Height):** `find_height` traverses the tree and returns \f$H = 6\f$. (This is 4 levels for the main AND-spine plus 2 levels for the longest OR-clause-spine, e.g., `(v1 + ~v2 + v3)`).

* **Task 5 (Truth Table):**
    * `get_Unique_Literals` finds \f$V = 8\f$ unique variables (v1...v8).
    * `printTruthTable` then begins printing the \f$2^8 = 256\f$ rows of the truth table.
    * This step clearly demonstrates the **inherent exponential bottleneck** \f$O(2^V)\f$.

* **Task 6 (CNF Conversion):**
    * The input formula is already in CNF.
    * `IMPL_FREE` finds no implications (`>`) and returns an identical tree.
    * `NNF` finds no negations to push inward and returns an identical tree.
    * `CNF` (and its helper `DISTR`) find no ORs to distribute over ANDs.
    * The final `cnf_root` is structurally identical to the initial `root`. In this case, the **algorithmic exponential bottleneck** \f$O(2^M)\f$ is not triggered.

* **Task 7 (Tautology Check):**
    * `checkCNFValidity` iterates through the 5 clause subtrees attached to the main AND-spine.
    * It calls `isClauseTautology` on each one.
    * For Clause 1 `(v1 + ~v2 + v3)`, the temporary hash table finds the set `{v1, ~v2, v3}`. No complementary pair (like `v1` and `~v1`) exists. The clause is not a tautology.
    * Since no individual clause is a tautology, the function correctly returns `false` (the entire formula is not a tautology).
---
