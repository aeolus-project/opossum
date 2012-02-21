
// translate a PSL problem in a MILP problem

#include <constraint_generation.h>


bool generate_desagregate_constraints = true;
bool generate_agregate_constraints = false;

int new_var = 0;
CUDFcoefficient min_bandwidth = 2; //TODO change value of min_bandwidth

struct SetPathCoeff {

public:

	SetPathCoeff(RankMapper* rankM, abstract_solver* solver) : rankM(rankM), solver(solver), stage(0) {

	}

	~SetPathCoeff() {
		rankM = NULL;
		solver = NULL;
	}
	inline unsigned int getStage() const { return stage; }
	inline void setStage(unsigned int stage) { this->stage = stage; };

	inline unsigned int getVarType() const { return varBorZ; }
	inline void setVarType(bool varBorZ) { this->varBorZ = varBorZ; };
	void operator()(FacilityNode* s, FacilityNode* d) {
		int rank = varBorZ ? rankM->rankB(s, d, stage) : rankM->rankZ(s, d, stage);
		solver->set_constraint_coeff(rank, 1);
		;
	}

private:
	RankMapper *rankM;
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
	RankMapper* rankM = new RankMapper(*problem);
	int nb_vars=rankM->size();//TODO   nb_vars = nb_packages + new_var;
	nb_vars = combiner.column_allocation(nb_vars);
	int other_vars=nb_vars - rankM->size();//TODO other_vars=nb_vars - nb_packages
	solver.init_solver(problem, other_vars);
	//TODO set_intvar_range(int rank, CUDFcoefficient lower, CUDFcoefficient upper);
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
	for(NodeIterator i = problem->getRoot()->nbegin() ; i!=  problem->getRoot()->nend() ; i++) {
		///////////
		//limit the number of servers at facilities
		for (int k = 0; k < problem->getNbServers(); ++k) {
			solver.new_constraint();
			solver.set_constraint_coeff( rankM->rankX(*i, k), 1);
			solver.add_constraint_leq(i->getType()->getServerCapacity(k));
		}
		///////////
		//limit the number of connections provided by facilities for a given stage
		for (int s = 0; s < problem->getNbGroups() + 1; ++s) {
			solver.new_constraint();
			solver.set_constraint_coeff( rankM->rankY(*i, s), -1);
			for (int k = 0; k < problem->getNbServers(); ++k) {
				solver.set_constraint_coeff( rankM->rankX(*i, k), problem->getServer(k)->getMaxConnections());
			}
			solver.add_constraint_geq(0);
		}
		///////////
		//connections flow conservation
		//TODO special case: initial broadcast (s=0)
		solver.new_constraint();
		if(! i->isRoot()) {
			solver.set_constraint_coeff( rankM->rankY(i->toFather(), 0), 1);
		}
		for(LinkListIterator l = i->cbegin(); l != i->cend() ; l++) {
			solver.set_constraint_coeff( rankM->rankY(*l, 0), -1);
		}
		//solver.add_constraint_eq(i->getType()->getDemand(s - 1)); //TODO define Xi or set directly coeffs of Xik ?
		//standard case: groups of clients
		for (int s = 1; s < problem->getNbGroups() + 1; ++s) {
			solver.new_constraint();
			if(! i->isRoot()) {
				solver.set_constraint_coeff( rankM->rankY(i->toFather(), s), 1);
			}
			for(LinkListIterator l = i->cbegin(); l != i->cend() ; l++) {
				solver.set_constraint_coeff( rankM->rankY(*l, s), -1);
			}
			solver.add_constraint_eq(i->getType()->getDemand(s - 1));
		}

	}

	SetPathCoeff setPC(rankM, &solver);
	///////////////////////
	//for each link ...
	///////////////////////
	///////////
	//for each stage ...
	for (int s = 0; s < problem->getNbGroups() + 1; ++s) {
		setPC.setStage(s);
		for(LinkIterator l = problem->getRoot()->lbegin() ; l!=  problem->getRoot()->lend() ; l++) {
			///////////
			//bandwidth passing through the link
			setPC.setVarType(true);
			solver.new_constraint();
			// TODO l->forEachPath();
			solver.add_constraint_leq(l->getBandwidth());

			///////////
			//number of connections passing through the link
			setPC.setVarType(false);
			solver.new_constraint();
			solver.set_constraint_coeff(rankM->rankY(*l, s), 1);
			//FIXME l->forEachPath(getV);
			//l->forEachPath(setPC);
			solver.add_constraint_eq(0);
		}
	}

	///////////////////////
	//for each path ...
	///////////////////////
	for(NodeIterator i = problem->getRoot()->nbegin() ; i!=  problem->getRoot()->nend() ; i++) {
		if( ! i->isLeaf()) {
			for(NodeIterator j = (i->nbegin()) ; j !=  i->nend() ; j++) {
				///////////
				//for each stage ...
				for (int s = 0; s < problem->getNbGroups() + 1; ++s) {
					///////////
					//minimal bandwidth for a single connection
					solver.new_constraint();
					solver.set_constraint_coeff(rankM->rankB(*i, *j, s), 1);
					solver.set_constraint_coeff(rankM->rankZ(*i, *j, s), - min_bandwidth);
					solver.add_constraint_geq(0);
				}
			}
		}
	}
	solver.end_add_constraints();
	//char* str = "/tmp/test.lp";
	//solver.writelp(str);
	return 0;
}

