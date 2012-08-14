/*******************************************************/
/* oPoSSuM solver: bandw_criteria.c                    */
/* Implementation of the bandw criteria                */
/* (c) Arnaud Malapert I3S (UNS-CNRS) 2012             */
/*******************************************************/

#include <bandw_criteria.h>


void bandw_criteria::initialize_upper_bound(PSLProblem *problem)
{
	_upper_bound = 0;
	for(LinkIterator i = problem->lbegin() ; i!=  problem->lend() ; i++) {
		_upper_bound += i->getBandwidth();
	}
}



int bandw_criteria::rank(const pair<FacilityNode*,FacilityNode*> & path, const unsigned int stage)
{
	return problem->rankB(path, stage);
}

