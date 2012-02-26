
/*******************************************************/
/* CUDF solver: new_criteria.h                         */
/* Implementation the new criteria                     */
/* (c) Claude Michel I3S (UNSA-CNRS) 2009,2010,2011    */
/*******************************************************/


#include <new_criteria.h>

// Criteria initialization
void new_criteria::initialize(PSLProblem *problem, abstract_solver *solver) {
	this->problem = problem;
	this->solver = solver;
	_upper_bound = 0;
	for(NodeIterator i = problem->nbegin() ; i!=  problem->nend() ; i++) {
		_upper_bound += i->getType()->getTotalCapacity();
	}
}

// Computing the number of columns required to handle the criteria
int new_criteria::set_variable_range(int first_free_var) {
	return first_free_var;
}

// Add the criteria to the current objective function
int new_criteria::add_criteria_to_objective(CUDFcoefficient lambda) {
	for(NodeIterator i = problem->nbegin() ; i!=  problem->nend() ; i++) {
		solver->set_obj_coeff(problem->rankX(*i), lambda_crit * lambda + solver->get_obj_coeff(problem->rankX(*i)));
	}
	return 0;
}

// Add the criteria to the constraint set
int new_criteria::add_criteria_to_constraint(CUDFcoefficient lambda) {
	for(NodeIterator i = problem->nbegin() ; i!=  problem->nend() ; i++) {
		solver->set_constraint_coeff(problem->rankX(*i), lambda_crit * lambda);
	}
	return 0;
}

// Add the constraints required by the criteria
int new_criteria::add_constraints() {
	return 0;
}

// Compute the criteria range
CUDFcoefficient new_criteria::bound_range() {
	return CUDFabs(lambda_crit) * _upper_bound;
}

// Compute the criteria upper bound
CUDFcoefficient new_criteria::upper_bound() { 
	return _upper_bound;
}

// Compute the criteria lower bound
CUDFcoefficient new_criteria::lower_bound() { 
	return 0;
}


