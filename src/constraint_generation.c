
// translate a PSL problem in a MILP problem

#include <constraint_generation.h>



int new_var = 0;
CUDFcoefficient min_bandwidth = 1; //set min_bandwidth to 1Ko

struct SetPathCoeff {

public:
	SetPathCoeff(PSLProblem* problem, abstract_solver* solver) : problem(problem), solver(solver), stage(0) {
	}

	~SetPathCoeff() {
		problem = NULL;
		solver = NULL;
	}
	inline unsigned int getStage() const { return stage; }
	inline void setStage(unsigned int stage) { this->stage = stage; };

	inline unsigned int getVarType() const { return varBorZ; }
	inline void setVarType(bool varBorZ) { this->varBorZ = varBorZ; };
	void operator()(FacilityNode* s, FacilityNode* d) {
		int rank = varBorZ ? problem->rankB(s, d, stage) : problem->rankZ(s, d, stage);
		solver->set_constraint_coeff(rank, 1);
		;
	}

private:
	PSLProblem* problem;
	abstract_solver *solver;
	unsigned int stage;
	bool varBorZ;

};

// Generate MILP objective function(s) and constraints for a given solver
// and a given criteria combination
int generate_constraints(PSLProblem *problem, abstract_solver &solver, abstract_combiner &combiner) {
	if ( ! problem->getRoot() ) { // we lack a problem then ...
		fprintf(stderr, "generate_constraints: no declared network !\n");
		exit(-1);
	}

	//----------------------------------------------------------------------------------------------------
	// Objective function
	int nb_vars=problem->rankCount();
	nb_vars = combiner.column_allocation(nb_vars);
	int other_vars=nb_vars - problem->rankCount();
	solver.init_solver(problem, other_vars);
	solver.begin_objectives();
	combiner.objective_generation();
	solver.end_objectives();

	//----------------------------------------------------------------------------------------------------
	// Constraints generation

	solver.begin_add_constraints();

	combiner.constraint_generation();
	//TODO access node/link iterator directly thru the problem
	///////////////////////
	//for each facility ...
	///////////////////////
	for(NodeIterator i = problem->nbegin() ; i!=  problem->nend() ; i++) {
		///////////
		//compute the total number of servers at facilities
		solver.new_constraint();
		solver.set_constraint_coeff( problem->rankX(*i), -1);
		for (int k = 0; k < problem->serverTypeCount(); ++k) {
			solver.set_constraint_coeff( problem->rankX(*i, k), 1);
		}
		solver.add_constraint_eq(0);
		///////////
		//limit the number of servers of a given type at facilities
		for (int k = 0; k < problem->serverTypeCount(); ++k) {
			solver.new_constraint();
			solver.set_constraint_coeff( problem->rankX(*i, k), 1);
			solver.add_constraint_leq(i->getType()->getServerCapacity(k));
		}
		///////////
		//limit the number of connections provided by facilities for a given stage
		for (int s = 0; s < problem->stageCount(); ++s) {
			solver.new_constraint();
			solver.set_constraint_coeff( problem->rankY(*i, s), -1);
			for (int k = 0; k < problem->serverTypeCount(); ++k) {
				solver.set_constraint_coeff( problem->rankX(*i, k), problem->getServer(k)->getMaxConnections());
			}
			solver.add_constraint_geq(0);
		}

		///////////
		//Additional constraints for the initial broadcast(s=0)
		if(i->isRoot()) {
			//the central facility contains the root pserver
			solver.new_constraint();
			solver.set_constraint_coeff( problem->rankX(*i, 0), 1);
			solver.add_constraint_geq(1);
		} else {
			//other facilities only receive the initial broadcast
			solver.new_constraint();
			solver.set_constraint_coeff( problem->rankY(*i, 0), 1);
			solver.add_constraint_eq(0);
		}


		///////////
		//connections flow conservation
		//special case: initial broadcast (s=0)
		solver.new_constraint();
		if(! i->isRoot()) {
			solver.set_constraint_coeff( problem->rankY(i->toFather(), 0), 1);
		}
		solver.set_constraint_coeff( problem->rankY(*i, 0), 1);
		for(LinkListIterator l = i->cbegin(); l != i->cend() ; l++) {
			solver.set_constraint_coeff( problem->rankY(*l, 0), -1);
		}
		solver.set_constraint_coeff( problem->rankX(*i), -1); //the pserver demand
		solver.add_constraint_eq(0);
		//standard case: groups of clients
		for (int s = 1; s < problem->stageCount(); ++s) {
			solver.new_constraint();
			if(! i->isRoot()) {
				solver.set_constraint_coeff( problem->rankY(i->toFather(), s), 1);
			}
			solver.set_constraint_coeff( problem->rankY(*i, s), 1);
			for(LinkListIterator l = i->cbegin(); l != i->cend() ; l++) {
				solver.set_constraint_coeff( problem->rankY(*l, s), -1);
			}
			solver.add_constraint_eq(i->getType()->getDemand(s - 1));
		}

	}

	SetPathCoeff setPC(problem, &solver);
	///////////////////////
	//for each link ...
	///////////////////////
	///////////
	//for each stage ...
	for (int s = 0; s < problem->groupCount() + 1; ++s) {
		setPC.setStage(s);
		for(LinkIterator l = problem->lbegin() ; l!=  problem->lend() ; l++) {
			///////////
			//bandwidth passing through the link
			setPC.setVarType(true);
			solver.new_constraint();
			l->forEachPath(setPC);
			solver.add_constraint_leq(l->getBandwidth());

			///////////
			//number of connections passing through the link
			setPC.setVarType(false);
			solver.new_constraint();
			solver.set_constraint_coeff(problem->rankY(*l, s), -1);
			l->forEachPath(setPC);
			solver.add_constraint_eq(0);
		}
	}

	///////////////////////
	//for each path ...
	///////////////////////

	for(NodeIterator i = problem->nbegin() ; i!=  problem->nend() ; i++) {
		if( ! i->isLeaf()) {
			NodeIterator j = i->nbegin();
			j++;
			while(j !=  i->nend()) {
				///////////
				//for each stage ...
				for (int s = 0; s < problem->stageCount(); ++s) {
					///////////
					//minimal bandwidth for a single connection
					solver.new_constraint();
					solver.set_constraint_coeff(problem->rankB(*i, *j, s), 1);
					solver.set_constraint_coeff(problem->rankZ(*i, *j, s), - min_bandwidth);
					solver.add_constraint_geq(0);
				}
				j++;
			}
		}
	}
	solver.end_add_constraints();
	return 0;
}

