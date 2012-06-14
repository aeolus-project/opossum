
/*******************************************************/
/* CUDF solver: conn_criteria.h                         */
/* Concrete class for the conn criteria               */
/* (c) Arnaud Malapert I3S (UNSA-CNRS) 2012            */
/*******************************************************/


#ifndef _CONN_CRITERIA_H_
#define _CONN_CRITERIA_H_

#include <abstract_criteria.h>

// A concrete class for the conn criteria
// i.e. the number of connections between a pserver and a client.
// the scope of connections can be retricted by using properties.
class conn_criteria: public pslp_criteria{
public:

	pair<unsigned int, unsigned int> stage_range;
	pair<unsigned int, unsigned int> length_range;

	unsigned int stage_max;

	conn_criteria(CUDFcoefficient lambda_crit, int reliable, pair<unsigned int, unsigned int> stage_range, pair<unsigned int, unsigned int> length_range) : pslp_criteria(lambda_crit, reliable), stage_range(stage_range), length_range(length_range) {};
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

	int rank(pair<FacilityNode*, FacilityNode*> const &path, const unsigned int stage);

private :

	inline bool isInRel(pair<FacilityNode*, FacilityNode*> const & path) {
		if(reliable < 0) return true;
		else {
			const bool relp = isReliablePath(path.first, path.second);
			return reliable == 0 ? !relp : relp;
		}
	}

	inline bool isInLength(pair<FacilityNode*, FacilityNode*> const & path) {
		return isInRange(path.second->getType()->getLevel() - path.first->getType()->getLevel(), length_range);
	}

	inline bool isInRL(pair<FacilityNode*, FacilityNode*> const &path) {
		return isInRel(path) && isInLength(path);
	}

};

#endif


