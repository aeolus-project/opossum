/*******************************************************/
/* oPoSSuM solver: cplex_solver.h                      */
/* Concrete class for the cplex solver                 */
/* (c) Arnaud malapert I3S (UNS-CNRS) 2012             */
/*******************************************************/


// concrete class which implements an interface to CPLEX solver

#ifndef _CPLEX_SOLVER_H
#define _CPLEX_SOLVER_H

#include <abstract_solver.h>
#include <scoeff_solver.h>
#include <ilcplex/cplex.h>

class cplex_solver: public abstract_solver, public scoeff_solver<double, 0, 0> {
public:
	// Solver initialization
	int init_solver(PSLProblem* problem, int other_vars);

	// Allocate some columns for integer variables
	int set_intvar_range(int rank, CUDFcoefficient lower, CUDFcoefficient upper);

	//set variable type to int and its range to [lower, upper] (must be used before end_objectives)
	int set_realvar_range(int rank, CUDFcoefficient lower, CUDFcoefficient upper);
	// set variable type to int and its range to [lower, upper] and its name to name (must be used before end_objectives)
	int set_intvar(int rank, char* name, CUDFcoefficient lower, CUDFcoefficient upper);
	// set variable type to real and its range to [lower, upper] and its name to name (must be used before end_objectives)
	int set_realvar(int rank, char* name, CUDFcoefficient lower, CUDFcoefficient upper);

	// set variable type to int and its range to [0, +inf[ and its name to name (must be used before end_objectives)
	int set_intvar(int rank, char* name);
	// set variable type to real and its range to [0, +inf[ and its name to name (must be used before end_objectives)
	int set_realvar(int rank, char* name);
	// set variable type to bool and its name to name (must be used before end_objectives)
	int set_boolvar(int rank, char* name);

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

	// Write the lp on a file
	int writelp(char *filename);
	int writesol(char *filename);

	// Solve the problem
	int solve();
	// Get the objective value (final one)
	CUDFcoefficient objective_value();
	// Init solutions (required before calling get_solution)
	int init_solutions();
	// Get the solution for a column
	CUDFcoefficient get_solution(int k);
	// get the real solution for a column
	double get_real_solution(int k);
	// get the number of solutions found at the end of solving
	int solutionCount(){return _solutionCount;}
	// get the number of objectives (or sub-problems).
	int objectiveCount();
	// get the number of nodes at the end of solving
	int nodeCount() {return _nodeCount;}
	// get the solving time.
	double timeCount() {return _timeCount;}

	// variables only for internal use (should be private)
	CPXENVptr env;   // cplex environment
	CPXLPptr lp;     // cplex linear program

	int first_objective;

	double *cplex_coeff; // cplex coefficients are doubles ...
	//pourquoi des double et pas des CUDFcoefficient ? toujours pour cplex ...
	double *lb;          // array of lower bounds
	double *ub;          // array of upper bounds
	char *vartype;       // array of variable types
	char **varname;      // array of variable names

	// Store the solutions
	double *solution;

	// solver creation
	cplex_solver(void) {
		solution = (double *)NULL;
	}

private:
	int _solutionCount;
	int _nodeCount;
	double _timeCount;

};

#endif
