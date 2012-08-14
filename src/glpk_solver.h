/*******************************************************/
/* oPoSSuM solver: glpk_solver.h                       */
/* Concrete class for the GLPK solver                  */
/* (c) Arnaud malapert I3S (UNS-CNRS) 2012             */
/*******************************************************/

// concrete class which implements an interface to GLPK solver

#ifdef USEGLPK

#ifndef _GLPK_SOLVER_H
#define _GLPK_SOLVER_H

#include <abstract_solver.h>
#include <scoeff_solver.h>
#include <glpk.h>

class glpk_solver: public abstract_solver, public scoeff_solver<double, 1, 1>  {
public:
	// Solver initialization
	int init_solver(PSLProblem* problem, int other_vars);

	// Does the solver use integer variables
	bool has_intvars();
	// Allocate some columns for integer variables
	int set_intvar_range(int rank, CUDFcoefficient lower, CUDFcoefficient upper);
	//set variable type to int and its range to [lower, upper] (must be used before end_objectives)
	int set_realvar_range(int rank, CUDFcoefficient lower, CUDFcoefficient upper); //TODO
	// set variable type to int and its range to [lower, upper] and its name to name (must be used before end_objectives)
	int set_intvar(int rank, char* name, CUDFcoefficient lower, CUDFcoefficient upper); //TODO
	// set variable type to real and its range to [lower, upper] and its name to name (must be used before end_objectives)
	int set_realvar(int rank, char* name, CUDFcoefficient lower, CUDFcoefficient upper); //TODO
	// set variable type to int and its range to [0, +inf[ and its name to name (must be used before end_objectives)
	int set_intvar(int rank, char* name);
	// set variable type to real and its range to [0, +inf[ and its name to name (must be used before end_objectives)
	int set_realvar(int rank, char* name);
	// set variable type to bool and its name to name (must be used before end_objectives)
	int set_boolvar(int rank, char* name); //TODO
	// Write the lp on a file
	int writelp(char *filename);

	// Solve the problem
	int solve();
	// Get the objective value (final one)
	CUDFcoefficient objective_value();
	// Init solutions (required before calling get_solution)
	int init_solutions();


	// Init the objective function definitions
	int begin_objectives(void);
	// Get current objective coefficient of a column
	CUDFcoefficient get_obj_coeff(int rank);
	// Set current objective coefficient of column
	int set_obj_coeff(int rank, CUDFcoefficient value);
	// Begin the definition of a new objective
	int new_objective(void);
	// Add current objective to the set of objectives
	int add_objective(void);
	// End objective definitions
	int end_objectives(void);

	// Init constraint definitions
	int begin_add_constraints(void);
	// Begin the definition of a new constraint
	int new_constraint(void);
	// Get current constraint coefficient of a column
	CUDFcoefficient get_constraint_coeff(int rank);
	// Set current constraint coefficient of a column
	int set_constraint_coeff(int rank, CUDFcoefficient value);
	// Add current constraint as a more or equal constraint
	int add_constraint_geq(CUDFcoefficient bound);
	// Add current constraint as a less or equal constraint
	int add_constraint_leq(CUDFcoefficient bound);
	// Add current constraint as a equality constraint
	int add_constraint_eq(CUDFcoefficient bound);
	// End constraint definitions
	int end_add_constraints(void);

	glp_prob *lp; // internal solver representation
	int nb_packages; // number of packages

	CUDFcoefficient *lb, *ub;          // arrays of lower and upper bounds

	// solver creation
	glpk_solver(bool use_exact) {
		lp = (glp_prob *)NULL;
		//all_versioned_packages = (CUDFVersionedPackageList *)NULL;
		lb = ub = (CUDFcoefficient *)NULL;
	}
};

#endif

#endif
