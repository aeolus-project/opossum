
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

#include <cudf_types.h>
#include "network.hpp"


using namespace std;

typedef vector<CUDFcoefficient> CUDFCoefficientList;
typedef vector<CUDFcoefficient>::iterator CUDFCoefficientIterator;

// current CUDF problem
extern PSLProblem *the_problem;
//
// parse the CUDF problem from input_file
extern int parse_pslp(istream& in);

//cudf_tools.c
//------------------------------------------------------------------
// Handling verbosity level
extern int verbosity;

// Print out a CUDF problem
// requires the file descriptor of the targetted file and a pointer to the CUDF problem
extern void print_problem(ostream& in, PSLProblem *pbs);

//cudf.y
//------------------------------------------------------------------

extern PSLProblem *current_problem;
//PSLProblem *the_problem = (PSLProblem *)NULL;              /* the CUDF problem to solve */



#endif
