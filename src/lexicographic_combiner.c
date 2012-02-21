
/*******************************************************/
/* CUDF solver: lexicographic_combiner.c               */
/* Implementation of the lexicographic combiner        */
/* (c) Claude Michel I3S (UNSA-CNRS) 2009,2010,2011    */
/*******************************************************/


#include <stdio.h>
#include <lexicographic_combiner.h>

// Compute the number of columns required to handle the combiner
int lexicographic_combiner::column_allocation(int first_rank) {
  for (CriteriaListIterator crit = criteria->begin(); crit != criteria->end(); crit++)
    first_rank = (*crit)->set_variable_range(first_rank);
  return first_rank;
}

// Generate the objective function
int lexicographic_combiner::objective_generation() {
  // Allow criteria to set the range of their integer variables
  for (CriteriaListIterator icrit = criteria->begin(); icrit != criteria->end(); icrit++) (*icrit)->initialize_intvars();

  for (CriteriaListIterator crit = criteria->begin(); crit != criteria->end(); crit++) {
    solver->new_objective();
    (*crit)->add_criteria_to_objective(+1);
    solver->add_objective();
  }
  return 0;
}

// Ask to criteria to generate their own constraints
int lexicographic_combiner::constraint_generation() {
  for (CriteriaListIterator crit = criteria->begin(); crit != criteria->end(); crit++)
    (*crit)->add_constraints();
  return 0;
}

// Does the combiner/criteria allows problem reduction
bool lexicographic_combiner::can_reduce() {
  bool result = true;
  for (CriteriaListIterator crit = criteria->begin(); crit != criteria->end(); crit++)
    result = result && (*crit)->can_reduce(+1);
  return result;
}

// Combiner initialization
void lexicographic_combiner::initialize(PSLProblem *problem, abstract_solver *solver) {
  this->solver = solver;
  for (CriteriaListIterator crit = criteria->begin(); crit != criteria->end(); crit++) (*crit)->initialize(problem, solver);
}
