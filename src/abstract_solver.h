
/*******************************************************/
/* CUDF solver: abstract_solver.h                      */
/* Abstract class for solvers                          */
/* (c) Claude Michel I3S (UNSA-CNRS) 2009,2010,2011    */
/*******************************************************/

// Solver abstraction
// all implemented solvers are implemented through a concrete class that inherits and implements an abstract solver.

#ifndef _ABSTRACT_SOLVER_H
#define _ABSTRACT_SOLVER_H

#include <cudf.h>
#include <stdarg.h>


// provide an abstraction of the underlying solvers
class abstract_solver {
public:
	// ******************************************************************
	// solver initialisation method (called at the beginning of constraint generation)
	virtual int init_solver(PSLProblem *problem, int other_vars) { return 0; };

	// ******************************************************************
	// does the solver has the capability to handle integer variables
	virtual bool has_intvars() { return false; };

	// set variable type to int and its range to [lower, upper] (must be used before end_objectives)
	virtual int set_intvar_range(int rank, CUDFcoefficient lower, CUDFcoefficient upper) { return 0; }
	// set variable type to int and its range to [lower, upper] (must be used before end_objectives)
	virtual int set_realvar_range(int rank, CUDFcoefficient lower, CUDFcoefficient upper) { return 0; }

	// set variable type to int and its range to [lower, upper] and its name to name (must be used before end_objectives)
	virtual int set_intvar(int rank, char* name, CUDFcoefficient lower, CUDFcoefficient upper) { return 0; }
	// set variable type to real and its range to [lower, upper] and its name to name (must be used before end_objectives)
	virtual int set_realvar(int rank, char* name, CUDFcoefficient lower, CUDFcoefficient upper) { return 0; }

	// set variable type to int and its range to [0, +inf[ and its name to name (must be used before end_objectives)
	virtual int set_intvar(int rank, char* name) { return 0; }
	// set variable type to real and its range to [0, +inf[ and its name to name (must be used before end_objectives)
	virtual int set_realvar(int rank, char* name) { return 0; }
	// set variable type to bool and its name to name (must be used before end_objectives)
	virtual int set_boolvar(int rank, char* name) { return 0; }


	// ******************************************************************
	// called before objective definitions
	virtual int begin_objectives(void) { return 0; };

	// get the current coefficient value of the objective function for variable rank
	virtual CUDFcoefficient get_obj_coeff(int rank) { return 0; };

	// set objective coefficient value according to a rank (column number)
	virtual int set_obj_coeff(int rank, CUDFcoefficient value) { return 0; };

	// called before defining a new objective
	virtual int new_objective(void) { return 0; };

	// called to add a new objective
	virtual int add_objective(void) { return 0; };

	// called at the end of objective definitions
	virtual int end_objectives(void) { return 0; };

	// ******************************************************************
	// constraint generation

	// called before the definition of any constraint
	virtual int begin_add_constraints(void) { return 0; };

	// called before the definition of a new constraint
	virtual int new_constraint(void) { return 0; };

	// get current constraint coefficient according to a rank
	virtual CUDFcoefficient get_constraint_coeff(int rank) { return 0; };

	// set constraint coefficient of a rank (i.e. column number)
	virtual int set_constraint_coeff(int rank, CUDFcoefficient value) { return 0; };

	// define the constraint as greater or equal to a bound (called once all constraints coefficients have been defined)
	virtual int add_constraint_geq(CUDFcoefficient bound) { return 0; };
	// define the constraint as less or equal to a bound (called once all constraints coefficients have been defined)
	virtual int add_constraint_leq(CUDFcoefficient bound) { return 0; };
	// define the constraint as equal to a bound (called once all constraints coefficients have been defined)
	virtual int add_constraint_eq(CUDFcoefficient bound) { return 0; };

	// called once all constraints have been defined
	virtual int end_add_constraints(void) { return 0; };


	// ******************************************************************
	// write the internal representation of the problem to a file
	virtual int writelp(char *filename) { return 0; };

	// ******************************************************************
	// solve the problem (must return a value > 0 if a solution has been found)
	virtual int solve() { return 0; };

	// ******************************************************************
	// initialisation of the solutions (called before reading them)
	virtual int init_solutions() { return 0; };

	// get the objective value at the end of solving
	virtual CUDFcoefficient objective_value() { return 0; };

	// get the status of a rank in the final configuration
	virtual CUDFcoefficient get_solution(int k) { return 0; };

	// ******************************************************************
	// abstract solver destructor
	virtual ~abstract_solver() {};

protected:

	virtual int init_vars(PSLProblem *problem, int nb_vars) {
		///////////////////////
		//for each facility ...
		for(NodeIterator i = problem->nbegin() ; i!=  problem->nend() ; i++) {
			//char buffer[20];
			set_intvar(problem->rankX(*i), sprint_var("x%d", i->getID()));
			for (int k = 0; k < problem->serverTypeCount(); ++k) {
				set_intvar(problem->rankX(*i, k), sprint_var("x%d_%d", i->getID(), k));
			}
			for (int s = 0; s < problem->stageCount(); ++s) {
				set_intvar(problem->rankY(*i, s), sprint_var("y%d'%d", i->getID(), s));
			}
		}

		///////////////////////
		//for each link ...
		for(LinkIterator i = problem->lbegin() ; i!=  problem->lend() ; i++) {
			for (int s = 0; s < problem->stageCount(); ++s) {
				set_intvar(problem->rankY(*i, s), sprint_var("y%d_%d'%d", i->getOrigin()->getID(), i->getDestination()->getID(), s));
			}
		}
		///////////////////////
		//for each path ...
		for(NodeIterator i = problem->nbegin() ; i!=  problem->nend() ; i++) {
			if( ! i->isLeaf()) {
				NodeIterator j = i->nbegin();
				j++;
				while(j !=  i->nend()) {
					for (int s = 0; s < problem->stageCount(); ++s) {
						set_intvar(problem->rankZ(*i, *j, s), sprint_var("z%d_%d'%d", i->getID(), j->getID(), s));
						set_realvar(problem->rankB(*i, *j, s), sprint_var("b%d_%d'%d", i->getID(), j->getID(), s));
					}
					j++;
				}
			}
		}

		// Set additional variable names
		for (int i = problem->rankCount(); i < nb_vars; i++) {
			set_intvar(i, sprint_var("X%d", i - problem->rankCount())); // Default Binary Variable
		}
		return 0;
	}

	inline char* sprint_var(const char * format, ...) {
		char buffer[16];
		va_list args;
		va_start(args, format);
		vsprintf(buffer, format, args);
		va_end(args);
		char *name;
		if ((name = (char *)malloc(strlen(buffer)+1)) == (char *)NULL) {
			fprintf(stderr, "CUDF error: can not alloc memory for variable name in cplex_solver::end_objective.\n");
			exit(-1);
		}
		strcpy(name, buffer);
		return name;
	}
};


#endif
