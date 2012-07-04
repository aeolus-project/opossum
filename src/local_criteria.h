/*******************************************************/
/* CUDF solver: local_criteria.h                       */
/* Concrete class for the local criteria               */
/* (c) Arnaud Malapert I3S (UNSA-CNRS) 2012            */
/*******************************************************/


#ifndef _LOCAL_CRITERIA_H_
#define _LOCAL_CRITERIA_H_

#include <abstract_criteria.h>

// A concrete class for the local criteria
// i.e. the number of local connections at facilities.
// the scope of facilities can be retricted by using properties.
class local_criteria: public pslp_criteria{
public:

	param_range level_range;
	param_range stage_range;

	// Criteria initialization
	void initialize(PSLProblem *problem, abstract_solver *solver);
	// Allocate some columns for the criteria
	int set_variable_range(int first_free_var);
	// Add the criteria to the objective
	int add_criteria_to_objective(CUDFcoefficient lambda);
	// Add the criteria to the constraint set
	int add_criteria_to_constraint(CUDFcoefficient lambda);
	// Add constraints required by the criteria
	int add_constraints();


	local_criteria(CUDFcoefficient lambda_crit, int reliable, param_range stage_range, param_range level_range) : pslp_criteria(lambda_crit, reliable), stage_range(stage_range), level_range(level_range) {}
	virtual ~local_criteria() {}

private :

	inline bool isRLSelected(FacilityNode* node) {
		if(level_range.contains(node->getType()->getLevel())) {
			return reliable == RELIABLE ? isReliablePath(problem->getRoot(), node) :
					reliable == NON_RELIABLE ? !isReliablePath(problem->getRoot(), node) : true;
		}
		return false;
	}

};

#endif


