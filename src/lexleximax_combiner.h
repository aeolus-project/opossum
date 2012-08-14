
/*******************************************************/
/* CUDF solver: lexleximax_combiner.h                  */
/* a concrete class for a lexleximax order combiner    */
/* (c) Claude Michel I3S (UNSA-CNRS) 2009,2010,2011    */
/*******************************************************/


#ifndef __LEXLEXIMAX_COMBINER_H
#define __LEXLEXIMAX_COMBINER_H

#include <abstract_combiner.h>

// A concrete class for a lexleximax order combiner
// Such a combiner first evaluates its first criteria according to a lexicographic order
// Then it evaluates the rest of the criteria according to a leximax order with respect to the first criteria
class lexleximax_combiner: public abstract_combiner {
 public:
  CriteriaList *criteria;   // set of criteria
  abstract_solver *solver;  // used solver

  int n, ui_n, yi_n, lambdaij_nn;
  CUDFcoefficient max_lambda;

  int column_allocation(int first_rank);

  // Generate combiner objective function
  int objective_generation();

  // Combiner constraint generation
  int constraint_generation();

  // Initialization
  void initialize(PSLProblem *problem, abstract_solver *solver);

  // Creation
  lexleximax_combiner(CriteriaList *criteria) { this->criteria = criteria; };
};

#endif
