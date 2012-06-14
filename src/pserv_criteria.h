
/*******************************************************/
/* CUDF solver: pserv_criteria.h                         */
/* Concrete class for the pserv criteria               */
/* (c) Arnaud Malapert I3S (UNSA-CNRS) 2012            */
/*******************************************************/


#ifndef _PSERV_CRITERIA_H_
#define _PSERV_CRITERIA_H_

#include <abstract_criteria.h>

// A concrete class for the pserv criteria
// i.e. the number of pservers that were
// not installed in the initial configuration and
// that are installed in the final one.
// The criteria can be restricted to a type of pservers.
class pserv_criteria: public pslp_criteria{
public:

	pair<unsigned int, unsigned int> pserv_range;
	pair<unsigned int, unsigned int> layer_range;

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


	pserv_criteria(CUDFcoefficient lambda_crit, int reliable, pair<unsigned int, unsigned int> pserv_range, pair<unsigned int, unsigned int> layer_range) : pslp_criteria(lambda_crit, reliable), pserv_range(pserv_range), layer_range(layer_range) {};

private :
	inline bool all_pserv() {
		return pserv_range.first <= 0 && problem->serverTypeCount() -1 <= pserv_range.second;
	}

	inline bool isInRel(FacilityNode* node) {
		if(reliable < 0) return true;
		else {
			const bool relp = isReliablePath(problem->getRoot(), node);
			return reliable == 0 ? !relp : relp;
		}
	}

	inline bool isInLayer(FacilityNode* node) {
		return isInRange(node->getType()->getLevel(), layer_range);
	}

	inline bool isInRL(FacilityNode* node) {
		return isInRel(node) && isInLayer(node);
	}

};

#endif


