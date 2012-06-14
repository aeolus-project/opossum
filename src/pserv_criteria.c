
/*******************************************************/
/* CUDF solver: new_criteria.h                         */
/* Implementation the pserv criteria                   */
/* (c) Arnaud Malapert I3S (UNSA-CNRS) 2012            */
/*******************************************************/


#include <pserv_criteria.h>


// Criteria initialization
void pserv_criteria::initialize(PSLProblem *problem, abstract_solver *solver) {
	this->problem = problem;
	this->solver = solver;
	_upper_bound = 0;

	if(all_pserv()) {
		for(NodeIterator i = problem->nbegin() ; i!=  problem->nend() ; i++) {
			if(isInRL(*i)) {
				_upper_bound += i->getType()->getTotalCapacity();
			}
		}
	} else {
		for(NodeIterator i = problem->nbegin() ; i!=  problem->nend() ; i++) {
			if(isInRL(*i)) {
				for (int k = pserv_range.first; k <= pserv_range.second; ++k) {
					_upper_bound += i->getType()->getServerCapacity(k);
				}
			}
		}
	}
}

// Computing the number of columns required to handle the criteria
int pserv_criteria::set_variable_range(int first_free_var) {
	return first_free_var;
}

// Add the criteria to the current objective function
int pserv_criteria::add_criteria_to_objective(CUDFcoefficient lambda) {
	if(all_pserv()) {
		for(NodeIterator i = problem->nbegin() ; i!=  problem->nend() ; i++) {
			if(isInRL(*i)) {
				set_obj_coeff(problem->rankX(*i), lambda);
			}
		}
	} else {
		for(NodeIterator i = problem->nbegin() ; i!=  problem->nend() ; i++) {
			if(isInRL(*i)) {
				for (int k = pserv_range.first; k <= pserv_range.second; ++k) {
					set_obj_coeff(problem->rankX(*i, k), lambda);
				}
			}
		}
	}
	return 0;
}

// Add the criteria to the constraint set
int pserv_criteria::add_criteria_to_constraint(CUDFcoefficient lambda) {
	if(all_pserv()) {
		for(NodeIterator i = problem->nbegin() ; i!=  problem->nend() ; i++) {
			if(isInRL(*i)) {
				solver->set_constraint_coeff(problem->rankX(*i), lambda_crit * lambda);
			}
		}
	} else {
		for(NodeIterator i = problem->nbegin() ; i!=  problem->nend() ; i++) {
			if(isInRL(*i)) {
				for (int k = pserv_range.first; k <= pserv_range.second; ++k) {
					set_constraint_coeff(problem->rankX(*i, k), lambda);
				}
			}
		}
	}
	return 0;
}

// Add the constraints required by the criteria
int pserv_criteria::add_constraints() {
	return 0;
}

// Compute the criteria range
CUDFcoefficient pserv_criteria::bound_range() {
	return CUDFabs(lambda_crit) * _upper_bound;
}

// Compute the criteria upper bound
CUDFcoefficient pserv_criteria::upper_bound() {
	return _upper_bound;
}

// Compute the criteria lower bound
CUDFcoefficient pserv_criteria::lower_bound() {
	return 0;
}


