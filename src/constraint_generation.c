
// translate a PSL problem in a MILP problem

#include <constraint_generation.h>


bool generate_desagregate_constraints = true;
bool generate_agregate_constraints = false;

int new_var = 0;



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

		//TODO special case: initial broadcast (s=0)

		//standard case: groups of clients
		for (int s = 1; s < problem->getNbGroups() + 1; ++s) {
			solver.new_constraint();
			solver.set_constraint_coeff( rankM->rankY(*i, s), -1);
			for (int k = 0; k < problem->getNbServers(); ++k) {
				solver.set_constraint_coeff( rankM->rankX(*i, k), problem->getServer(k)->getMaxConnections());
			}
			solver.add_constraint_geq(0);
		}
		///////////
		//connections flow conservation
		for (int s = 1; s < problem->getNbGroups() + 1; ++s) {
			solver.new_constraint();
			if(! i->isRoot()) {
				solver.set_constraint_coeff( rankM->rankY(i->toFather(), s), 1);
			}
			for(LinkListIterator l = i->cbegin(); l != i->cend() ; l++) {
				solver.set_constraint_coeff( rankM->rankY(*l, s), -1);
			}
			solver.add_constraint_eq(i->getType()->getDemand(s - 1)); //TODO check demand index
		}

	}
	///////////////////////
	//for each link ...
	///////////////////////
	for(LinkIterator l = problem->getRoot()->lbegin() ; l!=  problem->getRoot()->lend() ; l++) {
		///////////
		//for each stage
		for (int s = 0; s < problem->getNbGroups() + 1; ++s) {
			///////////
			//number of connections passing through the link
			solver.new_constraint();
			solver.set_constraint_coeff(rankM->rankY(*l, s), 1);
			// TODO l->forEachPath();
			//solver.set_constraint_coeff(rankM->rankZ(source, dest, s), -1);
			solver.add_constraint_eq(0);

			///////////
			//bandwidth passing through the link
			solver.new_constraint();
			// TODO l->forEachPath();
			//solver.set_constraint_coeff(rankM->rankB(source, dest, s), -1);
			solver.add_constraint_leq(l->getBandwidth());


		}
	}

	///////////////////////
	//for each path ...
	///////////////////////

	///////////
	//minimal bandwidth for a connection

	solver.end_add_constraints();
	//	char* str = "/tmp/test.lp";
	//	solver.writelp(str);
	return 0;
}
