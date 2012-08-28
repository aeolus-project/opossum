
/*******************************************************/
/* CUDF solver: abstract_combiner.h                    */
/* Abstract class for combiners                        */
/* (c) Claude Michel I3S (UNSA-CNRS) 2009,2010,2011    */
/*******************************************************/


#ifndef __ABSTRACT_COMBINER_H
#define __ABSTRACT_COMBINER_H

#include <abstract_criteria.h>

// An anstract combiner
class abstract_combiner {
 public:
  // Called to know the number of columns required by the combiner
  virtual int column_allocation(int first_rank) { return first_rank; }

  // Method in charge of the combiner objective generation
  virtual int objective_generation() { return 0; }

  // Method in charge of the combiner constraint generation
  virtual int constraint_generation() { return 0; }

  // Called to let the combiner initializes itself
  virtual void initialize(PSLProblem *problem, abstract_solver *solver) { };

  // Combiner destructor
  virtual ~abstract_combiner() { };
};

#endif
