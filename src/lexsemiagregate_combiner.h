
/*******************************************************/
/* CUDF solver: lexsemiagregate_combiner.h             */
/* a concrete class for a semi lexicographic agregate  */
/* combiner (i.e. agregate criteria 2 by 2)            */
/* (c) Claude Michel I3S (UNSA-CNRS) 2009,2010,2011    */
/*******************************************************/


#ifndef __LEXSEMIAGREGATE_COMBINER_H
#define __LEXSEMIAGREGATE_COMBINER_H

#include <abstract_combiner.h>

// A concrete class for a semi lexicographic agregate combiner
// Criteria are agregated 2 by 2 according to a lexicographic order
// while the resulting objective are evaluated according to a lexical order
// It allows to avoid objective range issues 
class lexsemiagregate_combiner: public abstract_combiner {
 public:
  CriteriaList *criteria;   // set of criteria
  abstract_solver *solver;  // used solver

  int column_allocation(int first_rank);

  int objective_generation();

  int constraint_generation();

  // lexsemiagregate initialization
  void initialize(PSLProblem *problem, abstract_solver *solver);

  // lexsemiagregate creation
  lexsemiagregate_combiner(CriteriaList *criteria) { this->criteria = criteria; };
};

#endif
