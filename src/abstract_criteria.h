
/*******************************************************/
/* CUDF solver: abstract_criteria.h                    */
/* Abstract class for criteria                         */
/* (c) Claude Michel I3S (UNSA-CNRS) 2009,2010,2011    */
/*******************************************************/



#ifndef _ABSTRACT_CRITERIA_H_
#define _ABSTRACT_CRITERIA_H_

#define  RELIABLE_OR_NOT -1
#define NON_RELIABLE 0
#define RELIABLE 1

#include <abstract_solver.h>

// Abstract criteria class
class abstract_criteria {
public:
	// Method called to allocate some variables (columns) to the criteria
	virtual int set_variable_range(int first_free_var) { return 0; }
	// Method called to add the criteria to the current objective
	virtual int add_criteria_to_objective(CUDFcoefficient lambda) { return 0; };
	// Method called to add the criteria to the constraints
	virtual int add_criteria_to_constraint(CUDFcoefficient lambda) { return 0; };
	// Method called to add criteria related constraints
	virtual int add_constraints() { return 0; };

	// Gives the range of the criteria objective
	virtual CUDFcoefficient bound_range() { return 0; };
	// Gives the upper bound of the criteria objective
	virtual CUDFcoefficient upper_bound() { return 0; };
	// Gives the lower bound of the criteria objective
	virtual CUDFcoefficient lower_bound() { return 0; };

	// Method called to let the criteria initializes itself
	virtual void initialize(PSLProblem *problem, abstract_solver *solver) { };
	// Method called to initialize criteria variables
	virtual void initialize_intvars() { };

	//Method called to let the criteria checks some properties availability
	virtual void check_property(PSLProblem *problem) {};

	// Criteria destructor
	virtual ~abstract_criteria() { };
};


// Type for a list of criteria
typedef vector<abstract_criteria *> CriteriaList;
typedef CriteriaList::iterator CriteriaListIterator;

// Shall we optimize variable usage or not
extern bool criteria_opt_var;

inline bool isInRange(unsigned int val, pair<unsigned int, unsigned int> &range) {
	return val >= range.first && val <= range.second;
}

// A generic class for defining PSLP criteria.
class pslp_criteria : public abstract_criteria {
public:
	PSLProblem *problem;      // a pointer to the problem
	abstract_solver *solver;   // a pointer to the solver

	// select elements according to their reliability (nodes, links, paths ...)
	int reliable;
	// lambda multiplier for the criteria
	CUDFcoefficient lambda_crit ;

	// upper bound of the criteria
	int _upper_bound;

	virtual void initialize(PSLProblem *problem, abstract_solver *solver) {
		this->problem = problem;
		this->solver = solver;
		_upper_bound = 0;
	}

	// Compute the criteria range, upper and lower bounds
	virtual CUDFcoefficient bound_range() {
		return CUDFabs(lambda_crit) * _upper_bound;
	}
	virtual CUDFcoefficient upper_bound() {
		return _upper_bound;
	}
	virtual CUDFcoefficient lower_bound() {
		return 0;
	}

	pslp_criteria(CUDFcoefficient lambda_crit, int reliable) : lambda_crit(lambda_crit), reliable(reliable)  {};

	// Criteria destructor
	virtual ~pslp_criteria() {};

protected :

	inline void set_constraint_coeff(int rank, CUDFcoefficient value) {
		solver->set_constraint_coeff(rank, lambda_crit * value);
	}

	inline void set_obj_coeff(int rank, CUDFcoefficient value) {
		solver->set_obj_coeff(rank, lambda_crit * value + solver->get_obj_coeff(rank));
	}
};


class param_range {

private:
	string param_name;
	int _min, _max;
public:
	param_range(string param, int min, int max) : param_name(param), _min(min), _max(max) {
		assert(_min <= _max);
	}
	param_range(string param, int min) : param_name(param), _min(min), _max(numeric_limits<int>::max()) {}
	param_range(string param) : param_name(param), _min(0), _max(numeric_limits<int>::max()) {}

	virtual ~param_range() {}

	bool contains(int val) {
		return _min <= val && val <= _max;
	}

	bool scanf(const char * s) {
		//return sscanf(s, (param_name + ":,%d-%d").c_str(), &_min, &_max) == 2;
		if(sscanf(s, (param_name + ",%d-%d").c_str(), &_min, &_max) != 2) {
			char c;
			if(sscanf(s, (param_name + ",%d%c").c_str(), &_min, &c) == 2 && c == '-') {
				_max = numeric_limits<int>::max();
			}
			else if(sscanf(s, (param_name + ",%d-").c_str(), &_min) == 1) {
				_max=_min;
			} else return false;
		}
		return true;
	}

	void set_min_limit(int min_limit) {
		if(min_limit > _min) {
			_min = min_limit;
		}
		assert(_min <= _max);
	}

	void set_max_limit(int max_limit) {
		if(max_limit < _max) {
			_max = max_limit;
		}
		assert(_min <= _max);
	}

	int min() {
		return _min;
	}

	int max() {
		return _max;
	}
};
#endif

