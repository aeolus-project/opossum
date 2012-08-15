/*******************************************************/
/* oPoSSum solver: pserv_criteria.h                    */
/* Implementation the pserv criteria                   */
/* (c) Arnaud Malapert I3S (UNS-CNRS) 2012             */
/*******************************************************/


#include <pserv_criteria.h>


// Criteria initialization
void pserv_criteria::initialize(PSLProblem *problem, abstract_solver *solver) {
	pserv_range.set_min_limit(0);
	pserv_range.set_max_limit(problem->serverTypeCount()-1);
	level_range.set_min_limit(0);
	pslp_criteria::initialize(problem, solver);
}

void pserv_criteria::initialize_upper_bound(PSLProblem *problem) {
	_upper_bound = 0;
		for(NodeIterator i = problem->nbegin() ; i!=  problem->nend() ; i++) {
				if(isRLSelected(*i)) {
					for (int k = pserv_range.min(); k <= pserv_range.max(); ++k) {
						_upper_bound += i->getType()->getServerCapacity(k);
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
	for(NodeIterator i = problem->nbegin() ; i!=  problem->nend() ; i++) {
			if(isRLSelected(*i)) {
				for (int k = pserv_range.min(); k <= pserv_range.max(); ++k) {
					set_obj_coeff(problem->rankX(*i, k), lambda);
				}
			}
		}
	return 0;
}

// Add the criteria to the constraint set
int pserv_criteria::add_criteria_to_constraint(CUDFcoefficient lambda) {
		for(NodeIterator i = problem->nbegin() ; i!=  problem->nend() ; i++) {
			if(isRLSelected(*i)) {
				for (int k = pserv_range.min(); k <= pserv_range.max(); ++k) {
					set_constraint_coeff(problem->rankX(*i, k), lambda);
				}
			}
		}
	return 0;
}

int pserv_criteria::add_constraints()
{
	return 0;
}





