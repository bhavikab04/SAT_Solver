/*
 @licstart  The following is the entire license notice for the JavaScript code in this file.

 The MIT License (MIT)

 Copyright (C) 1997-2020 by Dimitri van Heesch

 Permission is hereby granted, free of charge, to any person obtaining a copy of this software
 and associated documentation files (the "Software"), to deal in the Software without restriction,
 including without limitation the rights to use, copy, modify, merge, publish, distribute,
 sublicense, and/or sell copies of the Software, and to permit persons to whom the Software is
 furnished to do so, subject to the following conditions:

 The above copyright notice and this permission notice shall be included in all copies or
 substantial portions of the Software.

 THE SOFTWARE IS PROVIDED "AS IS", WITHOUT WARRANTY OF ANY KIND, EXPRESS OR IMPLIED, INCLUDING
 BUT NOT LIMITED TO THE WARRANTIES OF MERCHANTABILITY, FITNESS FOR A PARTICULAR PURPOSE AND
 NONINFRINGEMENT. IN NO EVENT SHALL THE AUTHORS OR COPYRIGHT HOLDERS BE LIABLE FOR ANY CLAIM,
 DAMAGES OR OTHER LIABILITY, WHETHER IN AN ACTION OF CONTRACT, TORT OR OTHERWISE, ARISING FROM,
 OUT OF OR IN CONNECTION WITH THE SOFTWARE OR THE USE OR OTHER DEALINGS IN THE SOFTWARE.

 @licend  The above is the entire license notice for the JavaScript code in this file
*/
var NAVTREE =
[
  [ "Logic_Assignment_SAT_Solver", "index.html", [
    [ "Logic in Computer Science Assignment Algorithm Analysis", "analysis_page.html", [
      [ "Overall Review and Summary", "analysis_page.html#overview_sec", [
        [ "Key Strengths", "analysis_page.html#review_strengths", null ],
        [ "Critical Performance and Memory Analysis", "analysis_page.html#review_bottlenecks", null ]
      ] ],
      [ "Analysis Variable Key", "analysis_page.html#key_sec", null ],
      [ "Full Pipeline Analysis (main.c)", "analysis_page.html#pipeline_sec", [
        [ "Time Complexity: \\(O(F + C \\cdot L_{\\text{max}}^2 \\cdot k + S_{\\text{total}} + 2^V \\cdot M \\cdot k + 2^M)\\)", "analysis_page.html#main_time", null ],
        [ "Space Complexity: \\(O(S_{\\text{total}} + M \\cdot k + V \\cdot k + 2^M)\\)", "analysis_page.html#main_space", null ]
      ] ],
      [ "Detailed Module Analysis", "analysis_page.html#modules_sec", [
        [ "Task 0: CNF to Infix (convertingCNFtoInput.c)", "analysis_page.html#task0_sec", null ],
        [ "Task 1: Infix to Prefix (task1.c)", "analysis_page.html#task1_sec", null ],
        [ "Task 2: Prefix to Tree (Task2.c)", "analysis_page.html#task2_sec", null ],
        [ "Task 3: Tree to Infix (Task3.c)", "analysis_page.html#task3_sec", null ],
        [ "Task 4: Tree Height (Task4.c)", "analysis_page.html#task4_sec", null ],
        [ "Task 5: Evaluation &amp; Truth Table (Task5.c)", "analysis_page.html#task5_sec", null ],
        [ "Task 6: CNF Conversion (Task6.c)", "analysis_page.html#task6_sec", null ],
        [ "Task 7: Tautology Check (Task7.c)", "analysis_page.html#task7_sec", null ]
      ] ],
      [ "Example Pipeline Trace (example.cnf)", "analysis_page.html#example_trace_sec", [
        [ "Input File (example.cnf)", "analysis_page.html#example_input_sec", null ],
        [ "Task 0: CNF to Infix", "analysis_page.html#example_task0_sec", null ],
        [ "Task 1: Infix to Prefix", "analysis_page.html#example_task1_sec", null ],
        [ "Task 2: Prefix to Tree", "analysis_page.html#example_task2_sec", null ],
        [ "Subsequent Analysis Tasks", "analysis_page.html#example_tasks3_7_sec", null ],
        [ "* Since no individual clause is a tautology, the function correctly returns <span class=\"tt\">false</span> (the entire formula is not a tautology).", "analysis_page.html#autotoc_md3", null ]
      ] ]
    ] ],
    [ "SAT_Solver", "md__r_e_a_d_m_e.html", null ],
    [ "Data Structures", "annotated.html", [
      [ "Data Structures", "annotated.html", "annotated_dup" ],
      [ "Data Structure Index", "classes.html", null ],
      [ "Data Fields", "functions.html", [
        [ "All", "functions.html", null ],
        [ "Variables", "functions_vars.html", null ]
      ] ]
    ] ],
    [ "Files", "files.html", [
      [ "File List", "files.html", "files_dup" ],
      [ "Globals", "globals.html", [
        [ "All", "globals.html", null ],
        [ "Functions", "globals_func.html", null ],
        [ "Variables", "globals_vars.html", null ],
        [ "Typedefs", "globals_type.html", null ],
        [ "Macros", "globals_defs.html", null ]
      ] ]
    ] ]
  ] ]
];

var NAVTREEINDEX =
[
"_main_8c.html"
];

var SYNCONMSG = 'click to disable panel synchronization';
var SYNCOFFMSG = 'click to enable panel synchronization';
var LISTOFALLMEMBERS = 'List of all members';