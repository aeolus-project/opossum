
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

// Print out a CUDF problem
// requires the file descriptor of the targeted file and a pointer to the PSL problem
extern void print_problem(ostream& in, PSLProblem *pbs);

//cudf.y
//------------------------------------------------------------------



#endif
