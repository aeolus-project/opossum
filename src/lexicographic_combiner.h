
/*******************************************************/
/* CUDF solver: lexicographic_combiner.h               */
/* a concrete class for a lexicographic combiner       */
/* (c) Claude Michel I3S (UNSA-CNRS) 2009,2010,2011    */
/*******************************************************/


#ifndef __LEXICOGRAPHIC_COMBINER_H
#define __LEXICOGRAPHIC_COMBINER_H

#include <abstract_combiner.h>

// A concrete class which evaluates its criteria in a lexicographic order
class lexicographic_combiner: public abstract_combiner {
 public:
  CriteriaList *criteria;   // set of criteria
  abstract_solver *solver;  // used solver

  int column_allocation(int first_rank);

  int objective_generation();

  int constraint_generation();

  // initialization
  void initialize(PSLProblem *problem, abstract_solver *solver);

  // lexicographic combiner creation
  lexicographic_combiner(CriteriaList *criteria) { this->criteria = criteria; };
};

#endif
