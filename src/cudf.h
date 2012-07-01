
/*******************************************************/
/* CUDF solver: cudf.h                                 */
/* Handling of CUDF problem files                      */
/* (c) Claude Michel I3S (UNSA-CNRS) 2009,2010,2011    */
/*******************************************************/


// main CUDF include file

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
//#include "abstract_solver.h"

using namespace std;

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

// Print out a PSL problem
// requires the file descriptor of the targeted file and a pointer to the PSL problem
extern void print_problem(ostream& out, PSLProblem *pbs);

// Export the PSL problem as a graphivz file (.dot)
// requires the file descriptor of the targeted file and a pointer to the PSL problem
extern void export_problem(PSLProblem *problem);

// Print out a generator (expected children, nodes, clientts ...);
// requires the file descriptor of the targeted file and a pointer to the PSL problem
extern void print_generator_summary(ostream& out, PSLProblem *pbs);

// Print out the generator data (matrices);
// requires the file descriptor of the targeted file and a pointer to the PSL problem
extern void print_generator_data(ostream& out, PSLProblem *pbs);


//

#endif
