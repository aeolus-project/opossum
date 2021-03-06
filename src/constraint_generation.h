/*******************************************************/
/* oPoSSuM solver: constraint_generation.h             */
/* constraint generation for PSL problems              */
/* (c) Arnaud Malapert I3S (UNS-CNRS) 2012             */
/*******************************************************/



#ifndef _CONSTRAINT_GENERATION_H_
#define _CONSTRAINT_GENERATION_H_

#include <abstract_solver.h>
#include <criteria.h>
#include <combiner.h>


extern int new_var;
extern CUDFcoefficient min_bandwidth;
extern CUDFcoefficient max_bandwidth;


// main function for constraint generation (translate a CUDF problem into MILP problem for a given solver and a given criteria)
extern int generate_constraints(PSLProblem *problem, abstract_solver &solver, abstract_combiner &combiner);


#endif

