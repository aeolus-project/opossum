/*******************************************************/
/* oPoSSuM solver: local_criteria.c                    */
/* Implementation of the local criteria                */
/* (c) Arnaud Malapert I3S (UNS-CNRS) 2012             */
/*******************************************************/


#include <local_criteria.h>


// Criteria initialization
void local_criteria::initialize(PSLProblem *problem, abstract_solver *solver) {
	pslp_criteria::initialize(problem, solver);
	stage_range.set_min_limit(0);
	stage_range.set_max_limit(problem->stageCount()-1);
	level_range.set_min_limit(0);
	_upper_bound = 0;
	for(NodeIterator i = problem->nbegin() ; i!=  problem->nend() ; i++) {
			if(isRLSelected(*i)) {
					_upper_bound += i->getType()->getTotalDemand();
			}
		}
}

// Computing the number of columns required to handle the criteria
int local_criteria::set_variable_range(int first_free_var) {
	return first_free_var;
}

// Add the criteria to the current objective function
int local_criteria::add_criteria_to_objective(CUDFcoefficient lambda) {
	for(NodeIterator i = problem->nbegin() ; i!=  problem->nend() ; i++) {
			if(isRLSelected(*i)) {
				for (int s = stage_range.min(); s <= stage_range.max(); ++s) {
					set_obj_coeff(problem->rankZ(*i, s), lambda);
				}
			}
		}
	return 0;
}

// Add the criteria to the constraint set
int local_criteria::add_criteria_to_constraint(CUDFcoefficient lambda) {
		for(NodeIterator i = problem->nbegin() ; i!=  problem->nend() ; i++) {
			if(isRLSelected(*i)) {
				for (int s = stage_range.min(); s <= stage_range.max(); ++s) {
					set_constraint_coeff(problem->rankZ(*i, s), lambda);
				}
			}
		}
	return 0;
}

int local_criteria::add_constraints()
{
	return 0;
}








