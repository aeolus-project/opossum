
/*******************************************************/
/* CUDF solver: leximin_combiner.c                     */
/* Implementation of the leximin combiner              */
/* (c) Claude Michel I3S (UNSA-CNRS) 2009,2010,2011    */
/*******************************************************/


#include <stdio.h>
#include <values.h>
#include <leximin_combiner.h>

// Compute the number of columns required to handle the combiner
int leximin_combiner::column_allocation(int first_rank) {
  // Number of criteria
  n = criteria->size();

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
int leximin_combiner::objective_generation() {
  int ui;
  CUDFcoefficient min_y = MAXLONG, max_y = MINLONG, min_ui, max_ui;

  // Declare criteria values as integer
  ui = ui_n;
  for (CriteriaListIterator crit = criteria->begin(); crit != criteria->end(); crit++) {
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

  // We want to maximise each yi
  for (int i = 0; i < n; i++) {
    solver->new_objective();
    solver->set_obj_coeff(yi_n + i, -1);
    solver->add_objective();
  }
  return 0;
}

// Ask to criteria to generate their own constraints
int leximin_combiner::constraint_generation() {

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

  // Add each criteria as a new constraint
  int ui = ui_n;
  for (CriteriaListIterator crit = criteria->begin(); crit != criteria->end(); crit++) {
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
void leximin_combiner::initialize(PSLProblem *problem, abstract_solver *solver) {
//  if (! solver->has_intvars()) {
//    fprintf(stderr, "leximin_combiner: initialize: leximin combiner requires integer variables.\n");
//    exit(-1);
//  }
  this->solver = solver;
  for (CriteriaListIterator crit = criteria->begin(); crit != criteria->end(); crit++) (*crit)->initialize(problem, solver);
}


// Compute the number of required columns when the combiner is used as a criteria
int leximin_combiner::set_variable_range(int first_free_var) { 
  return column_allocation(first_free_var);
}

// Initialize integer variables
void leximin_combiner::initialize_intvars() {
  int ui;
  CUDFcoefficient min_y = MAXLONG, max_y = MINLONG, min_ui, max_ui;

  // Declare criteria values as integer
  ui = ui_n;
  for (CriteriaListIterator crit = criteria->begin(); crit != criteria->end(); crit++) {
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

  // Initialize criteria intvars
  for (CriteriaListIterator crit = criteria->begin(); crit != criteria->end(); crit++) 
    (*crit)->initialize_intvars();
}

// Add the combiner as a criteria to the curent objective function
int leximin_combiner::add_criteria_to_objective(CUDFcoefficient lambda) {
  // We want to maximise each yi
  if (n > 0) {
    solver->set_obj_coeff(yi_n, -1);
    for (int i = 1; i < n; i++) {
      solver->add_objective();
      solver->new_objective();
      solver->set_obj_coeff(yi_n + i, -1);
    } // last objective is added by the underlying combiner
  }
  return 0;
}

// Add the combiner objective as a constraint
int leximin_combiner::add_criteria_to_constraint(CUDFcoefficient lambda) { return 0; }

// Add the required constraints (from the criteria set)
int leximin_combiner::add_constraints() { return constraint_generation(); }

// Compute the range of the combiner/criteria
CUDFcoefficient leximin_combiner::bound_range() {
  CUDFcoefficient range = 0;

  for (CriteriaList::reverse_iterator crit = criteria->rbegin(); crit != criteria->rend(); ++crit)
    range += CUDFabs(lambda_crit) * (*crit)->bound_range();
  return range;
}

// Compute the upper bound of the combiner/criteria
CUDFcoefficient leximin_combiner::upper_bound() {
  CUDFcoefficient ub = 0;

  for (CriteriaList::reverse_iterator crit = criteria->rbegin(); crit != criteria->rend(); ++crit)
    if (lambda_crit >= 0)
      ub += lambda_crit * (*crit)->upper_bound();
    else
      ub += lambda_crit * (*crit)->lower_bound();
  return ub;
}

// Compute the lower bound of the combiner/criteria
CUDFcoefficient leximin_combiner::lower_bound() {
  CUDFcoefficient lb = 0;

  for (CriteriaList::reverse_iterator crit = criteria->rbegin(); crit != criteria->rend(); ++crit)
    if (lambda_crit >= 0)
      lb += lambda_crit * (*crit)->lower_bound();
    else
      lb += lambda_crit * (*crit)->upper_bound();
  return lb;
}


