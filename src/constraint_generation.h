
/*******************************************************/
/* CUDF solver: constraint_generation.h                */
/* constraint generation for cudf problems             */
/* (c) Claude Michel I3S (UNSA-CNRS) 2009,2010,2011    */
/*******************************************************/

// 

#ifndef _CONSTRAINT_GENERATION_H_
#define _CONSTRAINT_GENERATION_H_

#include <abstract_solver.h>
#include <criteria.h>
#include <combiner.h>



extern bool generate_desagregate_constraints;
extern bool generate_agregate_constraints;


extern int new_var;


// available criteria
#define CRIT_DEFAULT 1
#define CRIT_CONSERVATIVE 2
#define CRIT_FRESHER 3
#define CRIT_SFRESHER 4
#define CRIT_FFRESHER 5
#define CRIT_TFRESHER 5
#define CRIT_PARANOID 6
#define CRIT_TRENDY   7
#define CRIT_TRENDY2  8
#define CRIT_LEXPARANOID  9
#define CRIT_LEXTRENDY   10
#define CRIT_LEXTRENDY2  11

// main function for constraint generation (translate a CUDF problem into MILP problem for a given solver and a given criteria)
extern int generate_constraints(PSLProblem *problem, abstract_solver &solver, abstract_combiner &combiner);

#endif

