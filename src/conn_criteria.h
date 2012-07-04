/*******************************************************/
/* CUDF solver: bandw_criteria.h                       */
/* Concrete class for the bandw criteria               */
/* (c) Arnaud Malapert I3S (UNSA-CNRS) 2012            */
/*******************************************************/


#ifndef _CONN_CRITERIA_H_
#define _CONN_CRITERIA_H_

#include <abstract_criteria.h>

// A concrete class for the conn criteria
// i.e. the number of connections.
class conn_criteria: public pslp_criteria{
public:

	param_range stage_range;
	param_range length_range;

	conn_criteria(CUDFcoefficient lambda_crit, int reliable, param_range stage_range, param_range length_range) : pslp_criteria(lambda_crit, reliable), stage_range(stage_range), length_range(length_range) {};
	virtual ~conn_criteria() {}


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

	virtual void initialize_upper_bound(PSLProblem *problem);
	virtual int rank(pair<FacilityNode*, FacilityNode*> const &path, const unsigned int stage);

private :

	inline bool isRLSelected(pair<FacilityNode*, FacilityNode*> const &path) {
		if(length_range.contains(path.second->getType()->getLevel() - path.first->getType()->getLevel())) {
			return reliable == RELIABLE ? isReliablePath(path.first, path.second) :
					reliable == NON_RELIABLE ? isReliablePath(path.first, path.second) : true;
		}
		return false;

	}

};

#endif


