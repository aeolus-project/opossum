
/*******************************************************/
/* CUDF solver: lexsemiagregate_combiner.c             */
/* Implementation of the lexsemiagregate combiner      */
/* (c) Claude Michel I3S (UNSA-CNRS) 2009,2010,2011    */
/*******************************************************/


#include <stdio.h>
#include <lexsemiagregate_combiner.h>

// Compute the number of columns required to handle the combiner
int lexsemiagregate_combiner::column_allocation(int first_rank) {
  for (CriteriaListIterator crit = criteria->begin(); crit != criteria->end(); crit++)
    first_rank = (*crit)->set_variable_range(first_rank);
  return first_rank;
}

// Generate the objective function
int lexsemiagregate_combiner::objective_generation() {
  int size = criteria->size();
  CUDFcoefficient lambda = +1;

  // Allow criteria to set the range of their integer variables
  for (CriteriaListIterator icrit = criteria->begin(); icrit != criteria->end(); icrit++) (*icrit)->initialize_intvars();

  for (int i = 0; i < size; i += 2) {
    lambda = +1;
    solver->new_objective();
    if (i + 1 < size) {
      (*criteria)[i+1]->add_criteria_to_objective(+1);
      lambda = (*criteria)[i+1]->bound_range();
      if (lambda == 0) lambda = 1;
    }
    (*criteria)[i]->add_criteria_to_objective(lambda);
    solver->add_objective();
  }

  if ((size > 1) && (size % 2 != 0)) {
    solver->new_objective();
    (*criteria)[size-1]->add_criteria_to_objective(+1);
    solver->add_objective();
  }

  return 0;
}

// Ask to criteria to generate their own constraints
int lexsemiagregate_combiner::constraint_generation() {
  for (CriteriaListIterator crit = criteria->begin(); crit != criteria->end(); crit++)
    (*crit)->add_constraints();
  return 0;
}

// Does the combiner/criteria allows problem reduction
bool lexsemiagregate_combiner::can_reduce() { 
  bool result = true;

  for (CriteriaListIterator crit = criteria->begin(); crit != criteria->end(); crit++) 
    result = result && (*crit)->can_reduce(+1);
  return result;
}

// Combiner initialization
void lexsemiagregate_combiner::initialize(PSLProblem *problem, abstract_solver *solver) {
  this->solver = solver;
  for (CriteriaListIterator crit = criteria->begin(); crit != criteria->end(); crit++) (*crit)->initialize(problem, solver);
}
