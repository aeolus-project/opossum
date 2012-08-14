
/*******************************************************/
/* CUDF solver: leximax_combiner.h                     */
/* a concrete class for a leximax order combiner       */
/* (c) Claude Michel I3S (UNSA-CNRS) 2009,2010,2011    */
/*******************************************************/


#ifndef __LEXIMAX_COMBINER_H
#define __LEXIMAX_COMBINER_H

#include <abstract_combiner.h>

// A concrete class which evaluates its criteria in a leximax order
class leximax_combiner: public abstract_combiner, public abstract_criteria {
 public:
  CriteriaList *criteria;   // set of criteria
  abstract_solver *solver;  // used solver

  int n, ui_n, yi_n, lambdaij_nn;
  CUDFcoefficient max_lambda;

  // ********************************************************
  // Seen as a combiner

  int column_allocation(int first_rank);

  int objective_generation();

  int constraint_generation();

  // ********************************************************
  // Seen as a criteria

  int set_variable_range(int first_free_var);
  void initialize_intvars();
  int add_criteria_to_objective(CUDFcoefficient lambda);
  int add_criteria_to_constraint(CUDFcoefficient lambda);
  int add_constraints();

  // computing combiner/criteria ranges/bounds
  CUDFcoefficient bound_range();
  CUDFcoefficient upper_bound();
  CUDFcoefficient lower_bound();


  // initialization
  void initialize(PSLProblem *problem, abstract_solver *solver);

  // lambda coefficient for the current combiner/criteria
  CUDFcoefficient lambda_crit ;

  // leximax combiner creation
  leximax_combiner(CriteriaList *criteria) { this->lambda_crit = 1; this->criteria = criteria; };
  leximax_combiner(CriteriaList *criteria, CUDFcoefficient lambda_crit) { 
    this->criteria = criteria;
    this->lambda_crit = lambda_crit; 
  };
};

#endif
