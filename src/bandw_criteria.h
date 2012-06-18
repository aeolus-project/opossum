/*******************************************************/
/* CUDF solver: bandw_criteria.h                       */
/* Concrete class for the bandw criteria               */
/* (c) Arnaud Malapert I3S (UNSA-CNRS) 2012            */
/*******************************************************/


#ifndef _BANDW_CRITERIA_H_
#define _BANDW_CRITERIA_H_

#include <conn_criteria.h>

// A concrete class for the bandw criteria
// i.e. the bandwidth allocated to connections.
class bandw_criteria: public conn_criteria{
public:

	bandw_criteria(CUDFcoefficient lambda_crit, int reliable, param_range stage_range, param_range length_range) : conn_criteria(lambda_crit, reliable, stage_range, length_range) {};
	virtual ~bandw_criteria() {}

	void initialize_upper_bound(PSLProblem *problem);
	int rank(pair<FacilityNode*, FacilityNode*> const &path, const unsigned int stage);

};

#endif


