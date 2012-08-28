/*******************************************************/
/* oPoSSuM solver: glpk_solver.c                       */
/* Interface to the GLPK solver                        */
/* (c) Arnaud malapert I3S (UNS-CNRS) 2012             */
/*******************************************************/

#ifdef USEGLPK

#include <math.h>
#include <glpk_solver.h>

#define OUTPUT_MODEL 0

// external function for solver creation
abstract_solver *new_glpk_solver(bool use_exact) { return new glpk_solver(use_exact); }

// solver initialisation
int glpk_solver::init_solver(PSLProblem* problem, int other_vars) {
	_solutionCount = 0;
	_nodeCount = 0;
	_timeCount = 0;

	// Coefficient initialization
	initialize_coeffs(problem->rankCount()+ other_vars);

	lp = glp_create_prob();
	glp_add_cols(lp, nb_vars);

	lb = (CUDFcoefficient *)malloc((nb_vars+1)*sizeof(CUDFcoefficient));
	ub = (CUDFcoefficient *)malloc((nb_vars+1)*sizeof(CUDFcoefficient));
	vartype = (int *)malloc((nb_vars+1)*sizeof(int));
	varname = (char **)malloc((nb_vars+1)*sizeof(char *));

	if ((lb  == (CUDFcoefficient *)NULL) ||
			(ub  == (CUDFcoefficient *)NULL) ||
			(vartype  == (int *)NULL) ||
			(varname  == (char **)NULL)) {
		fprintf(stderr, "glpk_solver: initialization: not enough memory.\n");
		exit(-1);
	}

	init_vars(problem, nb_vars);
	return 0;
}

// Set range of an integer variable
int glpk_solver::set_intvar_range(int rank, CUDFcoefficient lower, CUDFcoefficient upper) { 
	lb[rank+1] = lower;
	ub[rank+1] = upper;
	vartype[rank+1]= GLP_IV;
	return 0;
}


// set variable type to int and its range to [lower, upper] and its name to name (must be used before end_objectives)
int glpk_solver::set_intvar(int rank, char* name, CUDFcoefficient lower, CUDFcoefficient upper) {
	varname[rank+1]=name;
	return set_intvar_range(rank, lower, upper);
}

// set variable type to real and its range to [lower, upper] and its name to name (must be used before end_objectives)
int glpk_solver::set_realvar(int rank, char* name, CUDFcoefficient lower, CUDFcoefficient upper) {
	varname[rank+1]=name;
	lb[rank+1] = lower;
	ub[rank+1] = upper;
	vartype[rank+1] = GLP_CV;
	return 0;
}

// set variable type to int and its range to [0, +inf[ and its name to name (must be used before end_objectives)
int glpk_solver::set_intvar(int rank, char* name){
	return set_intvar(rank, name, 0, 0);
}
// set variable type to real and its range to [0, +inf[ and its name to name (must be used before end_objectives)
int glpk_solver::set_realvar(int rank, char* name) {
	return set_realvar(rank, name, 0, 0);
}

// set variable type to bool and its name to name (must be used before end_objectives)
int glpk_solver::set_boolvar(int rank, char* name) {
	varname[rank+1]=name;
	lb[rank+1] = 0;
	ub[rank+1] = 1;
	vartype[rank+1]= GLP_BV;
	return 0;
}


// write the problem into a file
int glpk_solver::writelp(char *filename) { glp_write_lp(lp, NULL, filename); return 0; }

void glpk_solver::callback(glp_tree *tree, void *info){
	glpk_solver* obj=(glpk_solver*) info;
	assert(obj);
	switch (glp_ios_reason(tree)) {
	// The best bound and the number of nodes change only when GLPK
	// branches, generates cuts or finds an integer solution.
	case GLP_IBINGO:
		obj->_solutionCount++;
		break;
	case GLP_ISELECT:
		// Get total number of nodes
		glp_ios_tree_size(tree, NULL, NULL, &(obj->_subNodeCount));
		break;
	//case GLP_IROWGEN:

	}
}

// solve the current lp problem
int glpk_solver::solve() {
	int status = 0, nb_objectives = objectives.size();
	glp_iocp mip_params;

	glp_init_iocp(&mip_params);
	mip_params.gmi_cuts = GLP_ON;
	mip_params.mir_cuts = GLP_ON;
	mip_params.cov_cuts = GLP_ON;
	mip_params.clq_cuts = GLP_ON;
	mip_params.presolve = GLP_ON;
	mip_params.binarize = GLP_OFF;
	/* MIP time limit */
	mip_params.tm_lim = time_limit * 1000;
	/* MIP node log display information */
	mip_params.msg_lev = verbosity >= SEARCH ? GLP_MSG_ALL : verbosity >= VERBOSE ? GLP_MSG_ON : GLP_MSG_OFF;
	/* we pass the address of the current object into glpk along with the callback function */
	mip_params.cb_func=glpk_solver::callback;
	mip_params.cb_info=this;

	time_t ptime = time(NULL);
	time_t ctime = ptime;
	for (int k = 0; k < nb_objectives; k++) {

		glp_cpx_basis(lp);
		ptime=ctime;
		status = glp_intopt(lp, &mip_params);
		ctime= time(NULL);
		_timeCount += difftime(ctime, ptime);
		_nodeCount += _subNodeCount;
		switch (status) {
		case 0: {
			switch (glp_mip_status(lp)) {
			case GLP_UNDEF:return _solutionCount > 0 ? SAT : UNKNOWN;
			case GLP_OPT:break;
			case GLP_FEAS:return SAT;
			case GLP_NOFEAS: return _solutionCount > 0 ? SAT : UNSAT;
			default:return ERROR;
			}
			break;
		}
		case GLP_ETMLIM: return _solutionCount > 0 ? SAT : UNKNOWN;
		default:
			return ERROR;
		}

		if (k + 1 < nb_objectives) {
			// Get objective value
			CUDFcoefficient objval = objective_value();

			if (verbosity >= DEFAULT) printf(">>> Objective %d value : "CUDFflags"\n", k, objval);

			// Reset objective i coefficients
			for (int i = 1; i < objectives[k]->nb_coeffs + 1; i++)
				glp_set_obj_coef(lp, objectives[k]->sindex[i], 0);

			// Set objective i+1 as the actual objective function
			for (int i = 1; i < objectives[k+1]->nb_coeffs + 1; i++)
				glp_set_obj_coef(lp, objectives[k+1]->sindex[i], objectives[k+1]->coefficients[i]);

			// Add objective i = objval constraint
			int irow = glp_add_rows(lp, 1);
			glp_set_row_bnds(lp, irow, GLP_FX, objval, objval);
			glp_set_mat_row(lp, irow, objectives[k]->nb_coeffs, objectives[k]->sindex, objectives[k]->coefficients);

			// Output model to file (when requested)
			if (verbosity >= VERBOSE) {
				char buffer[1024];
				sprintf(buffer, "cplexpb-%d.lp", k);
				writelp(buffer);
			}
		}
	}
	return OPTIMUM;
}

// get objective function value
CUDFcoefficient glpk_solver::objective_value() { return (CUDFcoefficient)nearbyint(glp_mip_obj_val(lp)); }

// solution initialisation
int glpk_solver::init_solutions() {
	int cur_numcols = glp_get_num_cols(lp);

	if (solution != (double *)NULL) free(solution);

	if ((solution = (double *)malloc((nb_vars+1)*sizeof(double))) == (double *)NULL) {
		fprintf (stderr, "glpk_solver: init_solutions: cannot get enough memory to store solutions.\n");
		exit(-1);
	}
	for (int k = 1; k < cur_numcols; ++k) {
		solution[k] = glp_mip_col_val(lp,k );
	}
	if (verbosity >= VERBOSE) {
		// Output model to file (when requested)
		writesol(C_STR("sol-glpk.txt"));
	}
	return 0;
}

CUDFcoefficient glpk_solver::get_solution(int k) {  return (CUDFcoefficient)nearbyint(solution[k+1]); }

// initialize objective function
int glpk_solver::begin_objectives(void) {
	glp_set_obj_dir(lp, GLP_MIN);  // Problem is minimization
	return 0;
}

// return the package coefficient of the objective function
CUDFcoefficient glpk_solver::get_obj_coeff(int rank) { return (CUDFcoefficient)get_coeff(rank); }

// set column coefficient to a value
int glpk_solver::set_obj_coeff(int rank, CUDFcoefficient value) { set_coeff(rank, value); return 0; }

// initialize an additional objective function
int glpk_solver::new_objective(void) {
	reset_coeffs();
	return 0;
}

// add an additional objective function
int glpk_solver::add_objective(void) {
	push_obj();
	return 0;
}

int glpk_solver::writesol(char *filename)
{
	return glp_print_mip(lp, filename);
}

int glpk_solver::make_var(int rank) {
	//cout << rank << " " <<varname[rank] << " [" << lb[rank] <<  "," << ub[rank] << "] -> " << vartype[rank] <<endl;
	glp_set_col_name(lp, rank, varname[rank]); // Set the colunm name
	int vtype = vartype[rank];
	int btype = GLP_DB;
	if(lb[rank]== 0 && ub[rank]== 0) btype = GLP_LO;
	if(vartype[rank] ==GLP_IV &&
			lb[rank]== 0 && ub[rank]== 1) vtype = GLP_BV;
	glp_set_col_bnds(lp, rank, btype, lb[rank], ub[rank]);
	glp_set_col_kind(lp, rank, vtype);
	return 0;
}
// finalize the objective function
int glpk_solver::end_objectives(void) {

	for (int i = 1; i <= nb_vars; i++) {
		make_var(i);
	}
	// Set objective 0 as the actual objective function
	for (int k = 1; k < objectives[0]->nb_coeffs + 1; k++) glp_set_obj_coef(lp, objectives[0]->sindex[k], objectives[0]->coefficients[k]);

	return 0;
}

// initialize constraints
int glpk_solver::begin_add_constraints(void) { return 0; }

// begin a new constraint
int glpk_solver::new_constraint(void) { reset_coeffs(); return 0; }

// get the package coefficient of the current constraint
CUDFcoefficient glpk_solver::get_constraint_coeff(int rank) { return (CUDFcoefficient)get_coeff(rank); }

// set column coefficient of the current constraint
int glpk_solver::set_constraint_coeff(int rank, CUDFcoefficient value) {
	set_coeff(rank, value);
	return 0;
}

// add current constraint as a greater or equal constraint
int glpk_solver::add_constraint_geq(CUDFcoefficient bound) {
	if (nb_coeffs > 0 ) {
		int irow = glp_add_rows(lp, 1);
		glp_set_row_bnds(lp, irow, GLP_LO, bound, 0);
		glp_set_mat_row(lp, irow, nb_coeffs, sindex, coefficients);
	}
	return 0;
}

// add current constraint as a less or equal constraint
int glpk_solver::add_constraint_leq(CUDFcoefficient bound) {
	if (nb_coeffs > 0 ) {
		int irow = glp_add_rows(lp, 1);
		glp_set_row_bnds(lp, irow, GLP_UP, 0, bound);
		glp_set_mat_row(lp, irow, nb_coeffs, sindex, coefficients);
	}
	return 0;
}

// add current constraint as an equality constraint
int glpk_solver::add_constraint_eq(CUDFcoefficient bound) {
	if (nb_coeffs > 0 ) {
		int irow = glp_add_rows(lp, 1);
		glp_set_row_bnds(lp, irow, GLP_FX, bound, bound);
		glp_set_mat_row(lp, irow, nb_coeffs, sindex, coefficients);
	}
	return 0;
}

// finalize constraints
int glpk_solver::end_add_constraints(void) {
	if (verbosity >= VERBOSE) writelp(C_STR("glpkpb.lp"));
	return 0;
}

int glpk_solver::objectiveCount()
{
	return objectives.size();
}
#endif
