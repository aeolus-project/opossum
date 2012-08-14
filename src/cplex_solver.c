/*******************************************************/
/* oPoSSuM solver: cplex_solver.c                      */
/* Interface to the CPLEX solver                       */
/* (c) Arnaud malapert I3S (UNS-CNRS) 2012             */
/*******************************************************/


#include <cplex_solver.h>
#include <math.h>

#define USEXNAME 0

// solver creation 
abstract_solver *new_cplex_solver() { return new cplex_solver(); }

// solver initialisation 
// requires the list of versioned packages and the total amount of variables (including additional ones)
int cplex_solver::init_solver(PSLProblem *problem, int other_vars) {
	int status;
	_solutionCount = 0;
	_nodeCount = 0;
	_timeCount = 0;

	// Coefficient initialization
	initialize_coeffs(problem->rankCount() + other_vars);

	/* Initialize the CPLEX environment */
	env = CPXopenCPLEX (&status);
	if ( env == NULL ) {
		char  errmsg[1024];
		fprintf (stderr, "Could not open CPLEX environment.\n");
		CPXgeterrorstring (env, status, errmsg);
		fprintf (stderr, "%s", errmsg);
		exit(-1);
	}

	/* Set the value of the time limit*/
	status = CPXsetdblparam (env, CPX_PARAM_TILIM, time_limit);
	if ( status ) {
		fprintf (stderr, "Failure to set the time limit, error %d.\n", status);
		exit(-1);
	}

	/* Enhance EPGAP to handle big values correctly */
	status = CPXsetdblparam (env, CPX_PARAM_EPGAP, 0.0);
	if ( status ) {
		fprintf (stderr, "Failure to set EPGAP, error %d.\n", status);
		exit(-1);
	}

	/* Limit the number of thread to 1 */
	status = CPXsetintparam (env, CPX_PARAM_THREADS, 1);
	if ( status ) {
		fprintf (stderr, "Failure to set thread limit to 1, error %d.\n", status);
		exit(-1);
	}

	if (verbosity >= DEFAULT) {
		/* Turn on output to the screen */
		status = CPXsetintparam (env, CPX_PARAM_SCRIND, CPX_ON);
		if ( status ) {
			fprintf (stderr, "Failure to turn on screen indicator, error %d.\n", status);
			exit(-1);
		}
		/* MIP node log display information */
		int verb = verbosity >= SEARCH ? 5 : verbosity >= VERBOSE ? 2 : 1;
		status = CPXsetintparam (env, CPX_PARAM_MIPDISPLAY, verb);
		//		int val = -1;
		//		CPXgetintparam (env, CPX_PARAM_MIPDISPLAY, &val);
		//		cerr <<  val << endl;
		if ( status ) {
			fprintf (stderr, "Failure to turn off presolve, error %d.\n", status);
			exit(-1);
		}

	}

	/* Create the problem. */
	lp = CPXcreateprob (env, &status, "lpex1");

	/* A returned pointer of NULL may mean that not enough memory
     was available or there was some other problem.  In the case of
     failure, an error message will have been written to the error
     channel from inside CPLEX.  In this example, the setting of
     the parameter CPX_PARAM_SCRIND causes the error message to
     appear on stdout.  */

	if ( lp == NULL ) {
		fprintf (stderr, "Failed to create LP.\n");
		exit(-1);
	}

	first_objective = 0;

	lb = (double *)malloc(nb_vars*sizeof(double));
	ub = (double *)malloc(nb_vars*sizeof(double));
	vartype = (char *)malloc(nb_vars*sizeof(char));
	varname = (char **)malloc(nb_vars*sizeof(char *));

	if ((lb  == (double *)NULL) ||
			(ub  == (double *)NULL) ||
			(vartype  == (char *)NULL) ||
			(varname  == (char **)NULL)) {
		fprintf(stderr, "cplex_solver: initialization: not enough memory.\n");
		exit(-1);
	}

	init_vars(problem, nb_vars);
	return 0;
}

// set integer variable range (must be used before end_objective)
int cplex_solver::set_intvar_range(int rank, CUDFcoefficient lower, CUDFcoefficient upper) { 
	lb[rank] = lower;
	ub[rank] = upper;
	vartype[rank] = CPX_INTEGER;
	return 0;
};

int cplex_solver::set_realvar_range(int rank, CUDFcoefficient lower, CUDFcoefficient upper) {
	lb[rank] = lower;
	ub[rank] = upper;
	vartype[rank] = CPX_CONTINUOUS;
	return 0;
}

// set variable type to int and its range to [lower, upper] and its name to name (must be used before end_objectives)
int cplex_solver::set_intvar(int rank, char* name, CUDFcoefficient lower, CUDFcoefficient upper) {
	varname[rank] = name;
	return set_intvar_range(rank, lower, upper);

}
// set variable type to real and its range to [lower, upper] and its name to name (must be used before end_objectives)
int cplex_solver::set_realvar(int rank, char* name, CUDFcoefficient lower, CUDFcoefficient upper) {
	varname[rank] = name;
	return set_realvar_range(rank, lower, upper);
}
// set variable type to int and its range to [0, +inf[ and its name to name (must be used before end_objectives)
int cplex_solver::set_intvar(int rank, char* name){
	varname[rank] = name;
	lb[rank] = 0;
	ub[rank] = CPX_INFBOUND;
	vartype[rank] = CPX_INTEGER;
	return 0;
}

// set variable type to real and its range to [0, +inf[ and its name to name (must be used before end_objectives)
int cplex_solver::set_realvar(int rank, char* name) {
	varname[rank] = name;
	lb[rank] = 0;
	ub[rank] = CPX_INFBOUND;
	vartype[rank] = CPX_CONTINUOUS;
	return 0;
}

// set variable type to bool and its name to name (must be used before end_objectives)
int cplex_solver::set_boolvar(int rank, char* name) {
	varname[rank] = name;
	lb[rank] = 0;
	ub[rank] = 1;
	vartype[rank] = CPX_BINARY;
	return 0;
}

// Just write the lp problem in a file
int cplex_solver::writelp(char *filename) { 
	return CPXwriteprob (env, lp, filename, NULL);
}

// Just write the solution in a file
int cplex_solver::writesol(char *filename) {
	return CPXsolwrite(env, lp, filename);
}



// solve the current problem
int cplex_solver::solve() {
	int nb_objectives = objectives.size();
	int mipstat, status;

	// Presolving the problem
	time_t ptime = time(NULL);
	if (CPXpresolve(env, lp, CPX_ALG_NONE)) return 0;
	time_t ctime = time(NULL);
	_timeCount += difftime(ctime, ptime);
	// Solve the objectives in a lexical order
	for (int i = first_objective; i < nb_objectives; i++) {
		ptime = ctime;
		// Solve the mip problem
		if (CPXmipopt (env, lp)) return ERROR;
		ctime = time(NULL);
		_solutionCount += CPXgetsolnpoolnumsolns(env, lp) + CPXgetsolnpoolnumreplaced(env, lp);
		_timeCount += difftime(ctime, ptime);
		_nodeCount += CPXgetnodecnt(env, lp);
		// Get solution status
		if ((mipstat = CPXgetstat(env, lp)) == CPXMIP_OPTIMAL) {
			if (i < nb_objectives - 1) {
				// Get next non empty objective
				// (must be done here to avoid conflicting method calls
				int previ = i, nexti, nexti_nb_coeffs = 0;

				for (; i < nb_objectives - 1; i++) {
					nexti = i + 1;
					nexti_nb_coeffs = objectives[nexti]->nb_coeffs;
					if (nexti_nb_coeffs > 0) break;
				}

				if (nexti_nb_coeffs > 0) { // there is one more objective to solve
					// Set objective constraint value to objval
					int index[1];
					double values[1];

					index[0] = previ;
					values[0] = objective_value();

					if (verbosity >= DEFAULT)
						printf(">>>> Objective value %d = %f\n", previ, values[0]);
					{
						int status, begin[2];
						double rhs[1];

						begin[0] = 0;
						rhs[0] = values[0]; //ub;
						int n = objectives[previ]->nb_coeffs;
						begin[1] = n - 1;
						status = CPXaddrows(env, lp, 0, 1, n, rhs, "E", begin, objectives[previ]->sindex, objectives[previ]->coefficients, NULL, NULL);
						if (status) {
							fprintf(stderr, "cplex_solver: end_objective: cannot add %d objective as constraint.\n", i);
							exit(-1);
						}
					}

					// Set the new objective value
					reset_coeffs();

					// Set previous objective coefficients to zero
					for (int k = 0; k < objectives[previ]->nb_coeffs; k++) set_coeff(objectives[previ]->sindex[k], 0);

					// Set next objective coefficients to their actual values
					for (int k = 0; k < nexti_nb_coeffs; k++) set_coeff(objectives[nexti]->sindex[k], objectives[nexti]->coefficients[k]);
					// Do set the next objective
					status = CPXchgobj(env, lp, nb_coeffs, sindex, coefficients);
					if ( status ) {
						fprintf (stderr,"Cannot change objective value.  Exiting...\n");
						exit(-1);
					}

					// Output model to file (when requested)
					if (verbosity >= VERBOSE) {
						char buffer[1024];
						sprintf(buffer, "cplexpb-%d.lp", i);
						writelp(buffer);
					}
				} else
					return OPTIMUM;
			} else
				return OPTIMUM;
		} else if( mipstat == CPXMIP_TIME_LIM_INFEAS ||
				mipstat == CPXMIP_TIME_LIM_FEAS) {
				return _solutionCount > 0 ? SAT : UNKNOWN;
		} else {
			if (verbosity >= DEFAULT)
				fprintf(stderr, "CPLEX solution status = %d\n", mipstat);
			return ERROR;
		}
	}

	return 0;
}

// return the objective value
CUDFcoefficient cplex_solver::objective_value() { 
	double objval;
	int  status = CPXgetobjval (env, lp, &objval);
	if (status) {
		fprintf (stderr,"No MIP objective value available.  Exiting...\n");
		exit(-1);
	}
	//  printf("Objective value = % 24.24e\n", objval);
	return (CUDFcoefficient)nearbyint(objval);
}

// solution initialisation
int cplex_solver::init_solutions() {
	int status;
	int cur_numcols = CPXgetnumcols (env, lp);

	if (solution != (double *)NULL) free(solution);

	if ((solution = (double *)malloc(nb_vars*sizeof(double))) == (double *)NULL) {
		fprintf (stderr, "cplex_solver: init_solutions: cannot get enough memory to store solutions.\n");
		exit(-1);
	}

	status = CPXgetx (env, lp, solution, 0, cur_numcols-1);
	if ( status ) {
		fprintf (stderr, "cplex_solver: init_solutions: failed to get solutions.\n");
		exit(-1);
	} else if (verbosity >= VERBOSE) {
		// Output model to file (when requested)
		writesol(C_STR("sol-cplex.xml"));
	}
	return 0;
}


CUDFcoefficient cplex_solver::get_solution(int k) {  return (CUDFcoefficient)nearbyint(solution[k]); }
double cplex_solver::get_real_solution(int k) {  return solution[k]; }

// initialize the objective function
int cplex_solver::begin_objectives(void) { 
	// Set Problem as a minimization problem
	CPXchgobjsen (env, lp, CPX_MIN);
	return 0;
}


// return the objective function coefficient of a rank
CUDFcoefficient cplex_solver::get_obj_coeff(int rank) { return (CUDFcoefficient)get_coeff(rank); }


// set the objective function coefficient of a ranked variable 
int cplex_solver::set_obj_coeff(int rank, CUDFcoefficient value) { set_coeff(rank, value); return 0; };

// initialize an additional objective function 
int cplex_solver::new_objective(void) {
	reset_coeffs();
	return 0;
}

// add an additional objective function
int cplex_solver::add_objective(void) { 
	push_obj();
	return 0;
}

// ends up objective function construction
int cplex_solver::end_objectives(void) {
	if (objectives.size() > 0) {
		int status = 0, nb_coeffs = 0;

		// Set the first objective as the actual objective
		for (int k = 0; k < nb_vars; k++) coefficients[k] = 0;
		for (; first_objective < (int)objectives.size(); first_objective++)
			if ((nb_coeffs = objectives[first_objective]->nb_coeffs) > 0) break;
		if (nb_coeffs > 0)
			for (int k = 0; k < nb_coeffs; k++)
				coefficients[objectives[first_objective]->sindex[k]] = objectives[first_objective]->coefficients[k];
		else
			if (first_objective == (int)objectives.size()) first_objective--; // So that we solve at least one pbs
		status = CPXnewcols (env, lp, nb_vars, coefficients, lb, ub, vartype, varname);
		if (status) {
			fprintf(stderr, "cplex_solver: end_objective: cannot create objective function.\n");
			exit(-1);
		}
	}

	return 0;
}

// initialize constraint declaration
int cplex_solver::begin_add_constraints(void) { return 0; }

// begin the declaration of a new constraint
int cplex_solver::new_constraint(void) { reset_coeffs(); return 0; }

// return the coefficient value of a package
//CUDFcoefficient cplex_solver::get_constraint_coeff(CUDFVersionedPackage *package) { return (CUDFcoefficient)get_coeff(package); }

// return the coefficient value of a package
CUDFcoefficient cplex_solver::get_constraint_coeff(int rank) { return (CUDFcoefficient)get_coeff(rank); }

// set the coeffcient value of a package
//int cplex_solver::set_constraint_coeff(CUDFVersionedPackage *package, CUDFcoefficient value) { set_coeff(package, value); return 0; }

// set the coefficient value of a ranked variable
int cplex_solver::set_constraint_coeff(int rank, CUDFcoefficient value) { set_coeff(rank, value); return 0; }

// add constraint under construction as a greater or equal constraint
int cplex_solver::add_constraint_geq(CUDFcoefficient bound) {
	if (nb_coeffs > 0) {
		int status, begin[2];
		double rhs[1];

		begin[0] = 0;
		begin[1] = nb_coeffs;
		rhs[0] = bound;

		status = CPXaddrows(env, lp, 0, 1, nb_coeffs, rhs, "G", begin, sindex, coefficients, NULL, NULL);
		if (status) {
			fprintf(stderr, "cplex_solver: add_constraint_geq: cannot create geq constraint.\n");
			exit(-1);
		}
	}
	return 0;
}

// add constraint under construction as a less or equal constraint
int cplex_solver::add_constraint_leq(CUDFcoefficient bound) {
	if (nb_coeffs > 0) {
		int status, begin[2];
		double rhs[1];

		begin[0] = 0;
		begin[1] = nb_coeffs;
		rhs[0] = bound;
		status = CPXaddrows(env, lp, 0, 1, nb_coeffs, rhs, "L", begin, sindex, coefficients, NULL, NULL);
		if (status) {
			fprintf(stderr, "cplex_solver: add_constraint_leq: cannot create leq constraint.\n");
			exit(-1);
		}
	}
	return 0;
}

// add constraint under construction as an equal constraint
int cplex_solver::add_constraint_eq(CUDFcoefficient bound) {
	if (nb_coeffs > 0) {
		int status, begin[2];
		double rhs[1];

		begin[0] = 0;
		begin[1] = nb_coeffs;
		rhs[0] = bound;
		status = CPXaddrows(env, lp, 0, 1, nb_coeffs, rhs, "E", begin, sindex, coefficients, NULL, NULL);
		if (status) {
			fprintf(stderr, "cplex_solver: add_constraint_eq: cannot create eq constraint.\n");
			exit(-1);
		}
	}
	return 0;
}

// ends up constraint declaration
int cplex_solver::end_add_constraints(void) { 
	if (verbosity >= VERBOSE) writelp(C_STR("cplexpb.lp"));
	return 0;
}

int cplex_solver::objectiveCount()
{
	return objectives.size();
}


