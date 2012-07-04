
/*******************************************************/
/* CUDF solver: conn_criteria.c                       */
/* Implementation of the conn criteria                 */
/* (c) Arnaud Malapert I3S (UNSA-CNRS) 2012            */
/*******************************************************/


#include <conn_criteria.h>


// Criteria initialization
void conn_criteria::initialize(PSLProblem *problem, abstract_solver *solver) {
	pslp_criteria::initialize(problem, solver);
	stage_range.set_min_limit(0);
	stage_range.set_max_limit(problem->stageCount() - 1);
	length_range.set_min_limit(1); //Ignore local connections
	initialize_upper_bound(problem);
}

// Criteria initialization
void conn_criteria::initialize_upper_bound(PSLProblem *problem) {
	_upper_bound = 0;
	for(NodeIterator i = problem->nbegin() ; i!=  problem->nend() ; i++) {
		_upper_bound += i->getType()->getTotalDemand();
	}
}


int conn_criteria::rank(pair<FacilityNode*,FacilityNode*> const &path, const unsigned int stage)
{
	return problem->rankZ(path, stage);
}

// Computing the number of columns required to handle the criteria
int conn_criteria::set_variable_range(int first_free_var) {
	return first_free_var;
}



// Add the criteria to the current objective function
int conn_criteria::add_criteria_to_objective(CUDFcoefficient lambda) {
	for (PathIterator p = problem->getRoot()->pbegin(); p != problem->getRoot()->pend(); ++p) {
		if(isRLSelected(*p)) {
			for (int s = stage_range.min(); s <= stage_range.max(); ++s) {
				set_obj_coeff(rank(*p, s), lambda);
			}
		}
	}
	return 0;
}

// Add the criteria to the constraint set
int conn_criteria::add_criteria_to_constraint(CUDFcoefficient lambda) {
	for (PathIterator p = problem->getRoot()->pbegin(); p != problem->getRoot()->pend(); ++p) {
		if(isRLSelected(*p)) {
			for (int s = stage_range.min(); s <= stage_range.max(); ++s) {
				set_constraint_coeff(rank(*p, s), lambda);
			}
		}
	}
	return 0;
}

int conn_criteria::add_constraints()
{
	return 0;
}









