
/*******************************************************/
/* CUDF solver: conn_criteria.h                         */
/* Implementation of the conn criteria                   */
/* (c) Arnaud Malapert I3S (UNSA-CNRS) 2012            */
/*******************************************************/


#include <conn_criteria.h>


// Criteria initialization
void conn_criteria::initialize(PSLProblem *problem, abstract_solver *solver) {
	pslp_criteria::initialize(problem, solver);
	stage_max = min(stage_range.second, problem->stageCount() - 1);
	//TODO Initialize upper bound
}

// Computing the number of columns required to handle the criteria
int conn_criteria::set_variable_range(int first_free_var) {
	return first_free_var;
}

int conn_criteria::rank(pair<FacilityNode*,FacilityNode*> const &path, const unsigned int stage)
{
	return problem->rankZ(path, stage);
}
// Add the criteria to the current objective function
int conn_criteria::add_criteria_to_objective(CUDFcoefficient lambda) {
	if(isInRange(0, length_range)) {
		for (NodeIterator n = problem->getRoot()->nbegin(); n != problem->getRoot()->nend(); ++n) {
			if(reliable != 0) { //local connections are reliable.
				for (int s = stage_range.first; s <= stage_max; ++s) {
					set_obj_coeff(problem->rankZ(*n, s), lambda);
				}
			}
		}
	}
	for (PathIterator p = problem->getRoot()->pbegin(); p != problem->getRoot()->pend(); ++p) {
		if(isInRL(*p)) {
			for (int s = stage_range.first; s <= stage_max; ++s) {
				set_obj_coeff(rank(*p, s), lambda);
			}
		}
	}
	return 0;
}

// Add the criteria to the constraint set
int conn_criteria::add_criteria_to_constraint(CUDFcoefficient lambda) {
	if(isInRange(0, length_range)) {
		for (NodeIterator n = problem->getRoot()->nbegin(); n != problem->getRoot()->nend(); ++n) {
			if(reliable != 0) { //local connections are reliable.
				for (int s = stage_range.first; s <= stage_max; ++s) {
					set_constraint_coeff(problem->rankZ(*n, s), lambda);
				}
			}
		}
	}
	for (PathIterator p = problem->getRoot()->pbegin(); p != problem->getRoot()->pend(); ++p) {
		if(isInRL(*p)) {
			for (int s = stage_range.first; s <= stage_max; ++s) {
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









