/*******************************************************/
/* oPoSSuM solver: cudf.h                              */
/* Handling of PSL problem files                       */
/* (c) Arnaud Malapert I3S (UNS-CNRS) 2012             */
/*******************************************************/


// main include file

#ifndef _CUDF_H
#define _CUDF_H

#include <stdio.h>
#include <string.h>
#include <stdlib.h>
#include <vector>
#include <set>
#include <map>
#include <string>
#include <iostream>

#include "network.hpp"

using namespace std;

// Handling the time limit per subproblem
extern double time_limit;
;
//Solver status
#define ERROR 0
#define UNKNOWN 1
#define UNSAT 2
#define SAT 3
#define OPTIMUM 4



#define C_STR( text ) ((char*)std::string( text ).c_str())

// current CUDF problem
extern PSLProblem *the_problem;
extern PSLProblem *current_problem;
//
// parse the CUDF problem from input_file
extern int parse_pslp(istream& in);

//cudf_tools.c
//------------------------------------------------------------------
// Handling verbosity level
extern int verbosity;
//Logging Levels and messages
#define SILENT -1
//XCSP Status and Objective
#define QUIET 0
//XCSP solution, diagnostics, configurations
#define DEFAULT 1
//Print generator and export problems and solutions (graphviz + solver)
#define VERBOSE 2
//MIP node log display information
#define SEARCH 3
//Print network info
#define ALL 4


// Print out a PSL problem
// requires the file descriptor of the targeted file and a pointer to the PSL problem
extern void print_problem(ostream& out, PSLProblem *pbs);

// Export the PSL problem as a graphivz file (.dot)
// requires the file descriptor of the targeted file and a pointer to the PSL problem
extern void export_problem(PSLProblem *problem);

// Print out a generator (expected children, nodes, clients, raw data ...);
// requires the file descriptor of the targeted file and a pointer to the PSL problem
extern void print_generator_summary(ostream& out, PSLProblem *pbs);

#endif
