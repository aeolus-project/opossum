/*******************************************************/
/* oPoSSuM solver: lpsolve_solver.c                    */
/* Interface to the LPSOLVE solver                     */
/* (c) Arnaud malapert I3S (UNS-CNRS) 2012             */
/*******************************************************/

#ifdef USELPSOLVE

#include <lpsolve_solver.h>

#define OUTPUT_MODEL 0

// external creation of the solver
abstract_solver *new_lpsolve_solver() { return new lpsolve_solver(); }

// solver initialisation
int lpsolve_solver::init_solver(PSLProblem *problem, int other_vars) {
  nb_packages = 0;//all_versioned_packages->size();

  // Coefficient initialization
  initialize_coeffs(nb_packages + other_vars);

  //this->all_versioned_packages = all_versioned_packages;
  if (verbosity > 0) printf("nb vars = %d\n", nb_vars);

  if ((lp = make_lp(0, nb_vars)) == NULL) {
    fprintf(stderr, "Cannot create the lpsolve solver.\n");
    exit(-1);
  }

  if (verbosity < 2) set_verbose(lp, -1);

  if ((lb = (CUDFcoefficient *)malloc((nb_vars+1)*sizeof(CUDFcoefficient))) == (CUDFcoefficient *)NULL) {
    fprintf(stderr, "glpk_solver: init_solver: not enough memory for lb.\n");
    exit(-1);
  }

  if ((ub = (CUDFcoefficient *)malloc((nb_vars+1)*sizeof(CUDFcoefficient))) == (CUDFcoefficient *)NULL) {
    fprintf(stderr, "glpk_solver: init_solver: not enough memory for ub.\n");
    exit(-1);
  }

  for (int i = 0; i <= nb_vars; i++) { lb[i] = 0; ub[i] = 1; }

  return 0;
}

// Set the range of an integer variable
int lpsolve_solver::set_intvar_range(int rank, CUDFcoefficient lower, CUDFcoefficient upper) { 
  lb[rank+1] = lower;
  ub[rank+1] = upper;
  return 0; 
}

// write the lp to a file
int lpsolve_solver::writelp(char *filename) { write_lp(lp, filename); return 0; }

// call to the lpsolve solver (bypassing name issues)
int lpsolve(lprec *lp) { return solve(lp); }

// solve the lp
int lpsolve_solver::solve() {
  int status = 0, nb_objectives = objectives.size();

  if (nb_objectives == 1) {

    set_presolve(lp, PRESOLVE_ROWS | PRESOLVE_COLS | PRESOLVE_LINDEP |
		 PRESOLVE_REDUCEMIP | PRESOLVE_REDUCEGCD | PRESOLVE_PROBEFIX | PRESOLVE_PROBEREDUCE, get_presolveloops(lp));
    status = lpsolve(lp);
    if ((status == OPTIMAL) || (status == SUBOPTIMAL) || (status == PRESOLVED)) return 1; else return 0;

  } else {

    lprec *lps = copy_lp(lp); // Using a copy seems to be the only way to make it work

    for (int k = 0; k < nb_objectives; k++) {

      set_presolve(lp, PRESOLVE_ROWS | PRESOLVE_COLS | PRESOLVE_LINDEP | 
		   PRESOLVE_REDUCEMIP | PRESOLVE_REDUCEGCD | PRESOLVE_PROBEFIX | PRESOLVE_PROBEREDUCE, get_presolveloops(lp));
      status = lpsolve(lp);

      if (k + 1 < nb_objectives) {
	CUDFcoefficient objval = objective_value();
	
	if (verbosity > 0) printf(">>> Objective %d value : "CUDFflags"\n", k, objval);
	
	// Build the objective function
	if (! set_obj_fnex(lps, objectives[k+1]->nb_coeffs, objectives[k+1]->coefficients, objectives[k+1]->sindex)) {
	  fprintf(stderr, "generate_milp: cannot create objective function.\n");
	  exit(-1);
	}

	set_add_rowmode(lps, TRUE);
	add_constraintex(lps, objectives[k]->nb_coeffs, objectives[k]->coefficients, objectives[k]->sindex, EQ, objval);
	set_add_rowmode(lps, FALSE);
	/*
	      set_constr_type(lps, k+1, EQ);
	      set_rh(lps, k+1, objval);
	*/
	lp = copy_lp(lps);
	if (OUTPUT_MODEL) write_lp(lp, (char *)"lpsolvepbs1.lp"); 
      }
    }
  }

  if ((status == OPTIMAL) || (status == SUBOPTIMAL) || (status == PRESOLVED)) return 1; else return 0;
}

// return the objective value
CUDFcoefficient lpsolve_solver::objective_value() { return (CUDFcoefficient)nearbyint(get_objective(lp)); }

// initialize the solutions
int lpsolve_solver::init_solutions() { return 0; }

// return the package status within final configuration
//CUDFcoefficient lpsolve_solver::get_solution(CUDFVersionedPackage *package) {
//  // get_ptr_variables gives a solution array that only takes into account columns left by the presolve.
//  // Thus, the only way to get all solutions is the following one ... no comment ...
//  return (CUDFcoefficient)nearbyint(get_var_primalresult(lp, get_Norig_rows(lp) + package->rank + 1));
//}

// initialize the objective function
int lpsolve_solver::begin_objectives(void) { 
  set_minim(lp);  // Problem is minimization
  return 0; 
}

// return the package coefficient of the objective value
//CUDFcoefficient lpsolve_solver::get_obj_coeff(CUDFVersionedPackage *package) { return (CUDFcoefficient)get_coeff(package); }

// return the package coefficient of the objective value
CUDFcoefficient lpsolve_solver::get_obj_coeff(int rank) { return (CUDFcoefficient)get_coeff(rank); }

// set the package coefficient of the objective value
//int lpsolve_solver::set_obj_coeff(CUDFVersionedPackage *package, CUDFcoefficient value) { set_coeff(package, value); return 0; }
// set column coefficient of the objective value
int lpsolve_solver::set_obj_coeff(int rank, CUDFcoefficient value) { set_coeff(rank, value); return 0; }

// initialize an additional objective function 
int lpsolve_solver::new_objective(void) {
  reset_coeffs();
  return 0;
}

// add an additional objective function
int lpsolve_solver::add_objective(void) { 
  push_obj(); 
  return 0;
}

// finalize the objective function
int lpsolve_solver::end_objectives(void) {

  // Build the objective function
  if (! set_obj_fnex(lp, objectives[0]->nb_coeffs, objectives[0]->coefficients, objectives[0]->sindex)) {
    fprintf(stderr, "generate_milp: cannot create objective function.\n");
    exit(-1);
  }

  // Use names for colunms
  set_use_names(lp, FALSE, TRUE);
//  for (CUDFVersionedPackageListIterator ipkg = all_versioned_packages->begin(); ipkg != all_versioned_packages->end(); ipkg++) {
//    int i = (*ipkg)->rank + 1;
//    if ((! set_bounds(lp, i, 0, 1)) ||
//	(! set_binary(lp, i, TRUE)) ||
//	(! set_col_name(lp, i, (*ipkg)->versioned_name))) {
//      fprintf(stderr, "generate_milp: cannot set type, bounds or name for objective function a.\n");
//      exit(-1);
//    }
//  }
  for (int i = nb_packages + 1; i <= nb_vars; i++) {
    char *name;
    char buffer[20];

    sprintf(buffer, "x%d", i);
    if ((name = (char *)malloc(strlen(buffer)+1)) == (char *)NULL) {
      fprintf(stderr, "CUDF error: can not alloc memory for variable name in glpk_solver::end_objective.\n");
      exit(-1);
    }
    strcpy(name, buffer);

    if ((lb[i] == 0) && (ub[i] == 1)) {
      if ((! set_bounds(lp, i, 0, 1)) ||
	  (! set_binary(lp, i, TRUE)) ||
	  (! set_col_name(lp, i, name))) {
	fprintf(stderr, "generate_milp: cannot set type, bounds or name for objective function b.\n");
	exit(-1);
      }
    } else {
      if ((! set_bounds(lp, i, lb[i], ub[i])) ||
	  (! set_int(lp, i, TRUE)) ||
	  (! set_col_name(lp, i, name))) {
	fprintf(stderr, "generate_milp: cannot set type, bounds or name for objective function c.\n");
	exit(-1);
      }
    }
  }
  /*
  for (unsigned int k = 0; k < objectives.size(); k++) {
    add_constraintex(lp, objectives[k]->nb_coeffs, objectives[k]->coefficients, objectives[k]->sindex, EQ, 0);
    set_constr_type(lp, k+1, FR);
  }
  */
  return 0;
}

// initialize constraints
int lpsolve_solver::begin_add_constraints(void) { 
  set_add_rowmode(lp, TRUE);
  return 0; 
}

// begin a new constraint
int lpsolve_solver::new_constraint(void) { reset_coeffs(); return 0; }

// get package coefficient of the constraint under construction
//CUDFcoefficient lpsolve_solver::get_constraint_coeff(CUDFVersionedPackage *package) { return (CUDFcoefficient)get_coeff(package); }

// get package coefficient of the constraint under construction
CUDFcoefficient lpsolve_solver::get_constraint_coeff(int rank) { return (CUDFcoefficient)get_coeff(rank); }

// set package coefficient of the constraint under construction
//int lpsolve_solver::set_constraint_coeff(CUDFVersionedPackage *package, CUDFcoefficient value) {
//  set_coeff(package, value);
//  return 0;
//}

// set package coefficient of the constraint under construction
int lpsolve_solver::set_constraint_coeff(int rank, CUDFcoefficient value) { 
  set_coeff(rank, value);
  return 0;
}

// add constraint under construction as a greater or equal constraint
int lpsolve_solver::add_constraint_geq(CUDFcoefficient bound) {
  if (nb_coeffs > 0) add_constraintex(lp, nb_coeffs, coefficients, sindex, GE, bound);
  return 0;
}

// add constraint under construction as a less or equal constraint
int lpsolve_solver::add_constraint_leq(CUDFcoefficient bound) {
  if (nb_coeffs > 0) add_constraintex(lp, nb_coeffs, coefficients, sindex, LE, bound);
  return 0;
}

// add constraint under construction as an equality constraint
int lpsolve_solver::add_constraint_eq(CUDFcoefficient bound) {
  if (nb_coeffs > 0) add_constraintex(lp, nb_coeffs, coefficients, sindex, EQ, bound);
  return 0;
}

// finalize constraints
int lpsolve_solver::end_add_constraints(void) { 
  set_add_rowmode(lp, FALSE);
  if (OUTPUT_MODEL) write_lp(lp, (char *)"lpsolvepbs.lp"); 
  return 0; 
}

#endif
