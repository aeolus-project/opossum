
/*******************************************************/
/* CUDF solver: lexleximin_combiner.c                  */
/* Implementation of the lexleximin combiner           */
/* (c) Claude Michel I3S (UNSA-CNRS) 2009,2010,2011    */
/*******************************************************/


#include <stdio.h>
#include <values.h>
#include <lexleximin_combiner.h>

// Compute the number of columns required to handle the combiner
int lexleximin_combiner::column_allocation(int first_rank) {
  // Number of leximin handled criteria
  n = criteria->size() - 1;

  // First, let each criteria get some room for they own purpose
  for (CriteriaListIterator crit = criteria->begin(); crit != criteria->end(); crit++)
    first_rank = (*crit)->set_variable_range(first_rank);

  // Need n variables to store criteria values
  ui_n = first_rank; first_rank += n;

  // Need n variables to store the yi i.e. actual objective values
  yi_n = first_rank; first_rank += n;

  // Need n*n variables to store the lambdaij values
  lambdaij_nn = first_rank; first_rank += n*n;

  return first_rank;
}

// Generate the objective function
int lexleximin_combiner::objective_generation() {
  int ui;
  CUDFcoefficient min_y = MAXLONG, max_y = MINLONG, min_ui, max_ui;

  // Declare criteria values as integer
  ui = ui_n;
  for (CriteriaListIterator crit = (criteria->begin())++; crit != criteria->end(); crit++) {
    min_ui = (*crit)->lower_bound();
    max_ui = (*crit)->upper_bound();
    solver->set_intvar_range(ui++, min_ui, max_ui);
    if (max_y < max_ui) max_y = max_ui;
    if (min_y > min_ui) min_y = min_ui;
  }
  max_lambda = max_y - min_y;

  // Need n variables to store the yi i.e. actual objective values
  for (int i = 0; i < n; i++)
    solver->set_intvar_range(yi_n+i, min_y, max_y);

  // Allow criteria to set the range of their integer variables
  for (CriteriaListIterator icrit = criteria->begin(); icrit != criteria->end(); icrit++) (*icrit)->initialize_intvars();

  // First criteria is handled lexicographically
  solver->new_objective();
  (*criteria)[0]->add_criteria_to_objective(+1);
  solver->add_objective();

  // We want to maximise each yi  
  for (int i = 0; i < n; i++) {
    solver->new_objective();
    solver->set_obj_coeff(yi_n + i, -1);
    solver->add_objective();
  }
  return 0;
}

// Ask to criteria to generate their own constraints
int lexleximin_combiner::constraint_generation() {

  // Generate the AtLeast constraints
  for (int i = 0; i < n; i++) {
    // add yi <= uj + lmax * lij i.e. - yi + uj + lmax * lij >= 0 
    for (int j = 0; j < n; j++) {
      solver->new_constraint();
      solver->set_constraint_coeff(yi_n+i, -1);
      solver->set_constraint_coeff(ui_n+j, +1);
      solver->set_constraint_coeff(lambdaij_nn+i*n+j, +max_lambda);
      solver->add_constraint_geq(0);
    }
    // add \sigma_{i = 1}^{n} lij <= n - k where k = n - i + 1 i.e. <= i - 1
    solver->new_constraint();
    for (int j = 0; j < n; j++) solver->set_constraint_coeff(lambdaij_nn+i*n+j, +1);
    solver->add_constraint_leq(i);
  }

  // Add each criteria handled by leximin as a new constraint
  int ui = ui_n;
  for (CriteriaListIterator crit = (criteria->begin())++; crit != criteria->end(); crit++) {
    solver->new_constraint();
    (*crit)->add_criteria_to_constraint(1);
    solver->set_constraint_coeff(ui++, -1);
    solver->add_constraint_eq(0);
  }

  // Let each criteria generates his own constraints
  for (CriteriaListIterator crit = criteria->begin(); crit != criteria->end(); crit++)
    (*crit)->add_constraints();

  return 0;
}

// Combiner initialization
void lexleximin_combiner::initialize(PSLProblem *problem, abstract_solver *solver) {
//  if (! solver->has_intvars()) {
//    fprintf(stderr, "lexleximin_combiner: initialize: lexleximin combiner requires integer variables.\n");
//    exit(-1);
//  }
  this->solver = solver;
  for (CriteriaListIterator crit = criteria->begin(); crit != criteria->end(); crit++) (*crit)->initialize(problem, solver);
}

// Does the combiner allows problem reduction
bool lexleximin_combiner::can_reduce() { 
  bool result = true;

  for (CriteriaListIterator crit = criteria->begin(); crit != criteria->end(); crit++) 
    result = result && (*crit)->can_reduce(+1);
  return result;
}


