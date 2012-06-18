
/*******************************************************/
/* CUDF solver: bandw_criteria.c                       */
/* Implementation of the bandw criteria                */
/* (c) Arnaud Malapert I3S (UNSA-CNRS) 2012            */
/*******************************************************/


#include <bandw_criteria.h>


// Criteria initialization
void bandw_criteria::initialize(PSLProblem *problem, abstract_solver *solver) {
	pslp_criteria::initialize(problem, solver);
	stage_range.set_min_limit(0);
	stage_range.set_max_limit(problem->stageCount() - 1);
	length_range.set_min_limit(1); //No bandwidth variables for local connections
	//TODO Initialize upper bound
}

// Computing the number of columns required to handle the criteria
int bandw_criteria::set_variable_range(int first_free_var) {
	return first_free_var;
}

int bandw_criteria::rank(pair<FacilityNode*,FacilityNode*> const &path, const unsigned int stage)
{
	return problem->rankB(path, stage);
}

// Add the criteria to the current objective function
int bandw_criteria::add_criteria_to_objective(CUDFcoefficient lambda) {
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
int bandw_criteria::add_criteria_to_constraint(CUDFcoefficient lambda) {

	for (PathIterator p = problem->getRoot()->pbegin(); p != problem->getRoot()->pend(); ++p) {
		if(isRLSelected(*p)) {
			for (int s = stage_range.min(); s <= stage_range.max(); ++s) {
				set_constraint_coeff(rank(*p, s), lambda);
			}
		}
	}
	return 0;
}

int bandw_criteria::add_constraints()
{
	return 0;
}









