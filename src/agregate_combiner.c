
/*******************************************************/
/* CUDF solver: agregate_combiner.c                    */
/* Implementation of the agregate combiner             */
/* (c) Claude Michel I3S (UNSA-CNRS) 2009,2010,2011    */
/*******************************************************/


#include <stdio.h>
#include <agregate_combiner.h>

// Compute the number of columns required to handle the agregate
int agregate_combiner::column_allocation(int first_rank) {
  for (CriteriaListIterator crit = criteria->begin(); crit != criteria->end(); crit++)
    first_rank = (*crit)->set_variable_range(first_rank);
  return first_rank;
}

// Generate the objective function
int agregate_combiner::objective_generation() {
  // Allow criteria to set the range of their integer variables
  for (CriteriaListIterator icrit = criteria->begin(); icrit != criteria->end(); icrit++) (*icrit)->initialize_intvars();

  solver->new_objective();
  add_criteria_to_objective(1);
  solver->add_objective();
  return 0;
}

// Ask to criteria to generate their own constraints
int agregate_combiner::constraint_generation() {
  for (CriteriaListIterator crit = criteria->begin(); crit != criteria->end(); crit++)
    (*crit)->add_constraints();
  return 0;
}

// Combiner initialization
void agregate_combiner::initialize(PSLProblem *problem, abstract_solver *solver) {
  this->solver = solver;
  for (CriteriaListIterator crit = criteria->begin(); crit != criteria->end(); crit++) (*crit)->initialize(problem, solver);
}

// Compute the number of required columns when the combiner is used as a criteria
int agregate_combiner::set_variable_range(int first_free_var) { 
  for (CriteriaListIterator crit = criteria->begin(); crit != criteria->end(); crit++) 
    first_free_var = (*crit)->set_variable_range(first_free_var);

  return first_free_var;
}

// Add the combiner as a criteria to the curent objective function
int agregate_combiner::add_criteria_to_objective(CUDFcoefficient lambda) { 
  for (CriteriaList::iterator crit = criteria->begin(); crit != criteria->end(); ++crit)
    (*crit)->add_criteria_to_objective(lambda_crit*lambda);
  return 0;
}

// Add the combiner objective as a constraint
int agregate_combiner::add_criteria_to_constraint(CUDFcoefficient lambda) {
  for (CriteriaList::iterator crit = criteria->begin(); crit != criteria->end(); ++crit)
    (*crit)->add_criteria_to_constraint(lambda_crit*lambda);
  return 0;
}

// Add the required constraints (from the criteria set)
int agregate_combiner::add_constraints() { return constraint_generation(); }

// Compute the range of the combiner/criteria
CUDFcoefficient agregate_combiner::bound_range() {
  CUDFcoefficient range = 0;

  for (CriteriaList::reverse_iterator crit = criteria->rbegin(); crit != criteria->rend(); ++crit)
    range += CUDFabs(lambda_crit) * (*crit)->bound_range();
  return range;
}

// Compute the upper bound of the combiner/criteria
CUDFcoefficient agregate_combiner::upper_bound() {
  CUDFcoefficient ub = 0;

  for (CriteriaList::reverse_iterator crit = criteria->rbegin(); crit != criteria->rend(); ++crit)
    if (lambda_crit >= 0)
      ub += lambda_crit * (*crit)->upper_bound();
    else
      ub += lambda_crit * (*crit)->lower_bound();
  return ub;
}

// Compute the lower bound of the combiner/criteria
CUDFcoefficient agregate_combiner::lower_bound() {
  CUDFcoefficient lb = 0;

  for (CriteriaList::reverse_iterator crit = criteria->rbegin(); crit != criteria->rend(); ++crit)
    if (lambda_crit >= 0)
      lb += lambda_crit * (*crit)->lower_bound();
    else
      lb += lambda_crit * (*crit)->upper_bound();
  return lb;
}

// Initialize integer variables (from criteria set)
void agregate_combiner::initialize_intvars() { 
  for (CriteriaListIterator crit = criteria->begin(); crit != criteria->end(); crit++) 
    (*crit)->initialize_intvars();
}
