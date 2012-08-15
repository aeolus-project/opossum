/*******************************************************/
/* oPoSSuM solver: pserv_criteria.h                    */
/* Concrete class for the pserv criteria               */
/* (c) Arnaud Malapert I3S (UNS-CNRS) 2012             */
/*******************************************************/


#ifndef _PSERV_CRITERIA_H_
#define _PSERV_CRITERIA_H_

#include <abstract_criteria.h>

// A concrete class for the pserv criteria
// i.e. the number of pservers that were
// not installed in the initial configuration and
// that are installed in the final one.
// the scope of pservers and facilities can be retricted by using properties.
class pserv_criteria: public pslp_criteria{
public:

	param_range pserv_range;
	param_range level_range;

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


	pserv_criteria(CUDFcoefficient lambda_crit, int reliable, param_range pserv_range, param_range level_range) : pslp_criteria(lambda_crit, reliable), pserv_range(pserv_range), level_range(level_range) {}
	virtual ~pserv_criteria() {}

protected :
	void initialize_upper_bound(PSLProblem *problem);
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


