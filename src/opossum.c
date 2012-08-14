/*******************************************************/
/* oPoSSuM solver: cudf.c                              */
/* Main for PSL problems                               */
/* (c) Arnaud Malapert I3S (UNS-CNRS) 2012             */
/*******************************************************/

#include <opossum.h>
#include <constraint_generation.h>
#include <criteria.h>
#include <combiner.h>
#include <sys/stat.h>
#include <errno.h>
#include <limits.h>
#include "graphviz.hpp"


#define HIERARCHIC true

PSLProblem* current_problem = NULL;
PSLProblem* the_problem = NULL;

int verbosity = DEFAULT;
double time_limit = 600; // 10 mn per subproblem

template <typename T>
T* makeCombiner(CriteriaList* criteria, char* name) {
	if (criteria->empty()) {
		fprintf(stderr, "ERROR: -%s option requires a list of criteria.\n", name);
		exit(-1);
	}
	return new T(criteria);

}
// underlying solver declaration
// allows using solvers withour having to include the whole solver classes


extern abstract_solver *new_lp_solver(char *lpsolver);

#ifdef USECPLEX 
extern abstract_solver *new_cplex_solver();
#endif
#ifdef USELPSOLVE 
extern abstract_solver *new_lpsolve_solver();
#endif
#ifdef USEGLPK
extern abstract_solver *new_glpk_solver(bool use_exact);
#endif

// print cudf help
void print_help() {
	fprintf(
			stderr,
			"This software is distributed under a modified BSD licence (see LICENCE file) and was\n"
			"oPoSSuM is a c++ library for solving the multiobjective package location server problem with a mathematical programming solver.\n"
			"oPoSSum was partially supported by the Agence National de la Recherche (Aeolus project -- ANR-2010-SEGI-013-01).\n"
	);
	fprintf(
			stderr,
			"Usual call: opossum -i <input_file> -o <outputfile> <criteria combiner>[<criteria>{, <criteria>}*] <solver option> <other options>?\n");
	fprintf(stderr, "file options:\n");
	fprintf(
			stderr,
			"\t-i <input_file>: set the input file to <input_file> (by default attempt to read on stdin)\n"
			"\t-o <output_file>: set the output file to <output_file> (by default write on stdout)\n");
	fprintf(stderr, "solver options:\n");
#ifdef USECPLEX
	fprintf(stderr, "\t-cplex: use IBM ILOG Cplex solver\n");
#endif
#ifdef USELPSOLVE
	fprintf(stderr, "\t-lpsolve: use lpsolve solver\n");
#endif
#ifdef USEGLPK
	fprintf(stderr, "\t-glpk: use glpk solver\n");
#endif
	fprintf(
			stderr,
			"\t-lp <lpsolver>: use lp (cplex format) solver <lpsolver> (tested with scip and cbc)\n");
	fprintf(stderr,
			"\t-nosolve: do not solve the problem (for debug purpose)\n");
	fprintf(stderr, "combining criteria:\n");
	fprintf(stderr, " -lexicographic[<lccriteria>{,<lccriteria>}*]\n");
	fprintf(stderr,
			"   with <lccriteria> ::= {+,-}leximax[<ccriteria>{,<ccriteria>}*] |\n");
	fprintf(
			stderr,"                         {+,-}leximin[<ccriteria>{,<ccriteria>}*] |\n");
	fprintf(stderr, "                         <ccriteria>\n");
	fprintf(stderr,
			"   with  <ccriteria> ::= {+,-}agregate[<ccriteria>{,<ccriteria>}*]{[lambda]}? |\n");
	fprintf(stderr,
			"                         {+,-}lexagregate[<ccriteria>{,<cccriteria>}*]{[lambda]}? |\n");
	fprintf(stderr, "                         <criteria>\n");
	fprintf(stderr,
			"   with   <criteria> ::= {+,-}pserv[<property:>,<value>]{[lambda]}? | # number of package servers \n");
	fprintf(stderr,
			"                                where [<level>,<range>] restricts to facilities located at given levels\n");
	fprintf(stderr,
			"                                      [<reliable>,<bool>] restricts to facilities for which the path from the central facility are (non-)reliable.\n");
	fprintf(stderr,
			"                                      [<type>,<range>] restricts to pservers with given types.\n");
	fprintf(stderr,
			"                                      where an integer value or an (unbounded) interval is specified by the regular expression: <range>::=int(-|-int)?.\n");
	fprintf(stderr,
			"                         {+,-}local[<property:>,<value>]{[lambda]}? | # number of local connections \n");
	fprintf(stderr,
			"                                with the same properties than pserv minus the type of pservers\n");
	fprintf(stderr,
			"                                      [<reliable>,<bool>] restricts to facilities for which the path from the central facility are (non-)reliable\n");
	fprintf(stderr,
			"                                      [<type>,<range>] restricts to pservers with given types\n");
	fprintf(stderr,
			"                                      where an integer value or an (unbounded) interval is specified by the regular expression: <range>::=int(-|-int)?.\n");
	fprintf(stderr,
			"                         {+,-}conn[<property:>,<value>]{[lambda]}? | # number of connections between facilities \n");
	fprintf(stderr,
			"                                where [<stage>,<range>] restricts to given stages of the broadcast\n");
	fprintf(stderr,
			"                                      [<length>,<range>] restricts to connections relying on path with given lengths\n");
	fprintf(stderr,
			"                                      [<reliable>,<bool>] restricts to connections relying on (non-)reliable paths\n");
	fprintf(stderr,
			"                         {+,-}bandw[<property:>,<value>]{[lambda]}? | # bandwidth allocated to connections \n");
	fprintf(stderr,
			"                                with the same properties than conn\n");
	fprintf(stderr,
			"  eg.: -lexicographic[-pserv,+local,+bandw]\n");
	fprintf(stderr,
			"  eg.: -lexicographic[-lexagregate[-agregate[-pserv[type,0][10],-pserv[type,1][18],-pserv[type,2][6]]\n");
	fprintf(stderr,
			"  WARNING: spaces can not be used within a criteria combination.\n");
	fprintf(stderr, "criteria combination shortcuts:\n");
	fprintf(stderr,
			" -lex[<lccriteria>{,<lccriteria>}*] equivalent to -lexicographic[<lccriteria>{,<lccriteria>}*]\n");
	fprintf(stderr,
			" -lexagregate[<ccriteria>{,<ccriteria>}*] equivalent to -lex[-lexagregate[<ccriteria>{,<ccriteria>}*]]\n");
	fprintf(stderr,
			" -lexsemiagregate[<ccriteria>{,<ccriteria>}*] equivalent to \n");
	fprintf(stderr,
			"                        -lex[-lexagregate[<ccriteria>,<ccriteria>],-lexagregate[<ccriteria>,<ccriteria>],...]\n");
	fprintf(stderr,
			" -agregate[<ccriteria>{,<ccriteria>}*] equivalent to -lex[-agregate[<ccriteria>{,<ccriteria>}*]]\n");
	fprintf(stderr,
			" -leximax[<ccriteria>{,<ccriteria>}*] equivalent to -lex[-leximax[<ccriteria>{,<ccriteria>}*]]\n");
	fprintf(stderr,
			" -leximin[<ccriteria>{,<ccriteria>}*] equivalent to -lex[-lexmin[<ccriteria>{,<ccriteria>}*]]\n");
	fprintf(stderr,
			" -lexleximax[<ccriteria>{,<ccriteria>}*] equivalent to -lex[<ccriteria>,-leximax[<ccriteria>{,<ccriteria>}*]]\n");
	fprintf(stderr,
			" -lexleximin[<ccriteria>{,<ccriteria>}*] equivalent to -lex[<ccriteria>,-leximin[<ccriteria>{,<ccriteria>}*]]\n");
	fprintf(stderr,
			"  eg.: -lex[-pserv,-leximax[-pserv[reliable,0],-pserv[type,1]]]\n");
	fprintf(stderr, "other options:\n");
	fprintf(stderr, "\t-t<n>: set the time limit per subproblem to n seconds\n");
	fprintf(stderr, "\t-v<n>: set verbosity level to n\n");
	fprintf(stderr, "\t-s<n>: set the seed for the problem generator to n\n");
	fprintf(stderr, "\t-id: print node IDs in graphviz\n");
	fprintf(stderr, "\t-h|-help|--help: print this help\n");

}

// Basic user defined criteria option handling
int get_criteria_options(char *crit_descr, unsigned int &pos, vector< pair<unsigned int, unsigned int> *> *opts) {
	//TODO Simplify method (remove opts ?)
	if (crit_descr[pos] == '[') {
		int nb_read = 0;
		unsigned int start = ++pos;

		for (; pos < strlen(crit_descr); pos++)
			switch(crit_descr[pos]) {
			case '[':
				crit_descr[pos] = '\0';
				fprintf(stderr, "ERROR: criteria options: found '[' within criteria options: %s.\n", crit_descr);
				exit(-1);
				break;
			case ']':
			{
				unsigned int length = pos - start;
				if (length == 0) {
					crit_descr[pos] = '\0';
					fprintf(stderr, "ERROR: criteria options: found empty criteria option: %s.\n", crit_descr);
					exit(-1);
				}
				opts->push_back(new pair<unsigned int, unsigned int>(start, length));
				nb_read++;
				pos++; // ignore ending ']'
				return nb_read;
			}
			break;
			case ',':
			{
				unsigned int length = pos - start;
				if (length == 0) {
					crit_descr[pos] = '\0';
					fprintf(stderr, "ERROR: criteria options: found empty criteria option: %s.\n", crit_descr);
					exit(-1);
				}
				opts->push_back(new pair<unsigned int, unsigned int>(start, length));
				nb_read++;
				start = ++pos;
			}
			break;
			}

		fprintf(stderr, "ERROR: criteria options: criteria options ended without an ending ']': %s.\n", crit_descr);
		exit(-1);
	}

	return 0;
}


CUDFcoefficient get_criteria_lambda(char *crit_descr, unsigned int & start,unsigned int & length, char sign) {
	CUDFcoefficient lambda = 1;
	for (unsigned int i = 0; i < length; i++)
		if ((crit_descr[start+i] < '0') || (crit_descr[start+i] > '9')) {
			crit_descr[start+i+1] = '\0';
			fprintf(stderr, "ERROR: criteria options: a lambda value must be an integer int: %s\n", crit_descr);
			exit(-1);

		}

	if (sscanf(crit_descr+start, CUDFflags, &lambda) != 1) {
		crit_descr[start+length+1] = '\0';
		fprintf(stderr, "ERROR: criteria options: a lambda value is espected here: %s\n", crit_descr);
		exit(-1);
	}
	if (sign == '+') lambda *= -1;
	return lambda;
}


// Get user defined weight for a criteria
CUDFcoefficient get_criteria_lambda(char *crit_descr, unsigned int &pos, char sign) {
	vector< pair<unsigned int, unsigned int> *> opts;

	int n = get_criteria_options(crit_descr, pos, &opts);

	if (n == 1) {
		return get_criteria_lambda(crit_descr, opts[0]->first, opts[0]->second, sign);
	} else if (n > 1) {
		crit_descr[pos] = '\0';
		fprintf(stderr, "ERROR: criteria options: a lambda value is espected here: %s\n", crit_descr);
		exit(-1);
	}
	return sign == '+' ? -1 : +1;
}


void get_criteria_properties(char *crit_descr, unsigned int &pos,
		param_range &param1, param_range &param2,
		int& reliable, CUDFcoefficient& lambda, char sign) {
	int n = 0;
	do {
		vector< pair<unsigned int, unsigned int> *> opts;
		n = get_criteria_options(crit_descr, pos, &opts); //TODO Simplify method ?
		if( n > 0 &&
				! param1.scanf(crit_descr+opts[0]->first) &&
				! param2.scanf(crit_descr+opts[0]->first) &&
				sscanf(crit_descr+opts[0]->first, "reliable,%d", &reliable) != 1 &&
				sscanf(crit_descr+opts[0]->first, CUDFflags, &lambda) != 1
		) {
			crit_descr[pos] = '\0';
			fprintf(stderr, "ERROR: criteria options: invalid format [<property>,<value>]: %s\n", crit_descr+opts[0]->first);
			exit(-1);
		}

	} while(n > 0);
	if(sign == '+') {lambda *=-1;}
}

// Process a user defined criteria
CriteriaList *process_criteria(char *crit_descr, unsigned int &pos, bool first_level, vector<abstract_criteria *> *criteria_with_property) {
	CriteriaList *criteria = new CriteriaList();

	if (crit_descr[pos] == '[') {
		for (pos += 1; pos < strlen(crit_descr) && crit_descr[pos] != ']';) {
			unsigned int sign, crit_name, crit_name_length;

			// check for criteria sense
			switch (crit_descr[pos]) {
			case '+':
			case '-':
				sign = pos++;
				crit_name = pos;
				break;
			default:
				fprintf(stderr, "ERROR: criteria options: a criteria description must begin with a sign which gives its sense (- = min, + = max): %s\n", crit_descr+pos);
				exit(-1);
				break;
			}

			// look for end of criteria name
			for (; pos < strlen(crit_descr); pos++) {
				char c = crit_descr[pos];
				if ((c == ',') || (c == '[') || (c == ']')) break;
			}
			crit_name_length = pos - crit_name;

			// handle criteria
			if (strncmp(crit_descr+crit_name, "pserv", crit_name_length) == 0) {
				param_range r1("type"), r2("level");
				int rel = RELIABLE_OR_NOT;
				CUDFcoefficient lambda = 1;
				get_criteria_properties(crit_descr, pos, r1, r2, rel, lambda, crit_descr[sign]
				);
				criteria->push_back(new pserv_criteria(lambda, rel, r1, r2));
			} else if (strncmp(crit_descr+crit_name, "local", crit_name_length) == 0) {
				param_range r1("stage"), r2("level");
				int rel = RELIABLE_OR_NOT;
				CUDFcoefficient lambda = 1;
				get_criteria_properties(crit_descr, pos, r1, r2, rel, lambda, crit_descr[sign]);
				criteria->push_back(new local_criteria(lambda, rel, r1, r2));
			} else if (strncmp(crit_descr+crit_name, "conn", crit_name_length) == 0) {
				param_range r1("stage"), r2("length",1);
				int rel = RELIABLE_OR_NOT;
				CUDFcoefficient lambda = 1;
				get_criteria_properties(crit_descr, pos, r1, r2, rel, lambda, crit_descr[sign]);
				criteria->push_back(new conn_criteria(lambda, rel, r1, r2));
			} else if (strncmp(crit_descr+crit_name, "bandw", crit_name_length) == 0) {
				param_range r1("stage"), r2("length",1);
				int rel = RELIABLE_OR_NOT;
				CUDFcoefficient lambda = 1;
				get_criteria_properties(crit_descr, pos, r1, r2, rel, lambda, crit_descr[sign]);
				criteria->push_back(new bandw_criteria(lambda, rel, r1, r2));
			} else if (strncmp(crit_descr+crit_name, "agregate", crit_name_length) == 0) {
				criteria->push_back(new agregate_combiner(process_criteria(crit_descr, pos, false, criteria_with_property),
						get_criteria_lambda(crit_descr, pos, crit_descr[sign])));
			} else if (strncmp(crit_descr+crit_name, "lexagregate", crit_name_length) == 0) {
				criteria->push_back(new lexagregate_combiner(process_criteria(crit_descr, pos, false, criteria_with_property),
						get_criteria_lambda(crit_descr, pos, crit_descr[sign])));
			} else if (strncmp(crit_descr+crit_name, "leximax", crit_name_length) == 0) {
				if (crit_descr[sign] == '-')
					criteria->push_back(new leximax_combiner(process_criteria(crit_descr, pos, false, criteria_with_property)));
				else
					criteria->push_back(new leximin_combiner(process_criteria(crit_descr, pos, false, criteria_with_property)));
			} else if (strncmp(crit_descr+crit_name, "leximin", crit_name_length) == 0) {
				if (crit_descr[sign] == '-')
					criteria->push_back(new leximin_combiner(process_criteria(crit_descr, pos, false, criteria_with_property)));
				else
					criteria->push_back(new leximax_combiner(process_criteria(crit_descr, pos, false, criteria_with_property)));
			} else {
				crit_descr[pos] = '\0';
				fprintf(stderr, "ERROR: criteria options: this is not a criteria: %s\n", crit_descr+crit_name);
				exit(-1);
			}

			if (crit_descr[pos] == ',') pos++; // skip comma
		}
	} else {
		fprintf(stderr, "ERROR: criteria options: a criteria list must begin with a '[': %s\n", crit_descr+pos);
		exit(-1);
	}

	pos++;
	return criteria;
}

// Handling user criteria definitions
CriteriaList *get_criteria(char *crit_descr, bool first_level, vector<abstract_criteria *> *criteria_with_property) {
	unsigned int pos = 0;
	return process_criteria(crit_descr, pos, first_level, criteria_with_property);
}

// main CUDF function

int main(int argc, char *argv[]) {
	ofstream output_file;
	abstract_solver *solver = (abstract_solver *) NULL;
	abstract_combiner *combiner = (abstract_combiner *) NULL;
	char* obj_descr;
	unsigned int* seed = NULL;
	bool nosolve = false;
	bool got_input = false;
	bool got_output = false;
	PSLProblem *problem;

	vector<abstract_criteria *> criteria_with_property; //TODO Remove useless list ?
	// parameter handling
	if (argc > 1) {
		for (int i = 1; i < argc; i++) {
			if (strcmp(argv[i], "-i") == 0) {
				i++;
				if (i < argc) {
					ifstream in;
					in.open(argv[i]);
					if (!in) {
						fprintf(stderr, "ERROR: cannot open file %s as input file.\n", argv[i]);
						exit(-1);
					} else {
						got_input = true;
						switch (parse_pslp(in))
						{
						case 0: break;
						case 1: fprintf(stderr, "ERROR: invalid input in problem.\n"); exit(-1);
						case 2: fprintf(stderr, "ERROR: parser memory issue.\n"); exit(-1);
						}
						in.close();
					}
				}
			} else if (strcmp(argv[i], "-o") == 0) {
				i++;
				if (i < argc) {
					output_file.open(argv[i],std::ios::out);
					if ( !output_file) {
						fprintf(stderr, "ERROR: cannot open file %s as ouput file.\n", argv[i]);
						exit(-1);
					}
					got_output=true;
				}
			} else if (strncmp(argv[i], "-t", 2) == 0) {
				sscanf(argv[i]+2, "%lf", &time_limit);
			} else if (strncmp(argv[i], "-v", 2) == 0) {
				sscanf(argv[i]+2, "%u", &verbosity);
			} else if (strncmp(argv[i], "-s",2) == 0) {
				unsigned int tmp;
				sscanf(argv[i]+2, "%u", &tmp);
				seed = &tmp;
				sscanf(argv[i]+2, "%u", &(*seed));
			} else if (strcmp(argv[i], "-id") == 0) {
				showID=true;
			} else if (strncmp(argv[i], "-lex[", 5) == 0) {
				CriteriaList *criteria = get_criteria(argv[i]+4, true, &criteria_with_property);
				combiner = makeCombiner<lexicographic_combiner>(criteria, C_STR("lexicographic"));
				obj_descr = argv[i];
			} else if (strncmp(argv[i], "-lexicographic[", 15) == 0) {
				CriteriaList *criteria = get_criteria(argv[i]+14, true, &criteria_with_property);
				combiner = makeCombiner<lexicographic_combiner>(criteria, C_STR("lexicographic"));
				obj_descr = argv[i];
			} else if (strncmp(argv[i], "-agregate[", 10) == 0) {
				CriteriaList *criteria = get_criteria(argv[i]+9, false, &criteria_with_property);
				combiner = makeCombiner<agregate_combiner>(criteria, C_STR("agregate"));
				obj_descr = argv[i];
			} else if (strncmp(argv[i], "-lexagregate[", 13) == 0) {
				CriteriaList *criteria = get_criteria(argv[i]+12, false, &criteria_with_property);
				combiner = makeCombiner<lexagregate_combiner>(criteria, C_STR("lexagregate"));
				obj_descr = argv[i];
			} else if (strncmp(argv[i], "-lexsemiagregate[", 17) == 0) {
				CriteriaList *criteria = get_criteria(argv[i]+16, false, &criteria_with_property);
				combiner = makeCombiner<lexsemiagregate_combiner>(criteria, C_STR("lexsemiagregate"));
				obj_descr = argv[i];
			} else if (strncmp(argv[i], "-leximax[", 9) == 0) {
				CriteriaList *criteria = get_criteria(argv[i]+8, false, &criteria_with_property);
				combiner = makeCombiner<leximax_combiner>(criteria, C_STR("leximax"));
				obj_descr = argv[i];
			} else if (strncmp(argv[i], "-leximin[", 9) == 0) {
				CriteriaList *criteria = get_criteria(argv[i]+8, false, &criteria_with_property);
				combiner = makeCombiner<leximin_combiner>(criteria, C_STR("leximin"));
				obj_descr = argv[i];
			} else if (strncmp(argv[i], "-lexleximax[", 12) == 0) {
				CriteriaList *criteria = get_criteria(argv[i]+11, false, &criteria_with_property);
				combiner = makeCombiner<lexleximax_combiner>(criteria, C_STR("lexleximax"));
				obj_descr = argv[i];
			} else if (strncmp(argv[i], "-lexleximin[", 12) == 0) {
				CriteriaList *criteria = get_criteria(argv[i]+11, false, &criteria_with_property);
				combiner = makeCombiner<lexleximin_combiner>(criteria, C_STR("lexleximin"));
				obj_descr = argv[i];
			} else if (strcmp(argv[i], "-h") == 0 || strcmp(argv[i], "-help") == 0 || strcmp(argv[i], "--help") == 0 ) {
				print_help();
				exit(-1);
			} else if (strcmp(argv[i], "-nosolve") == 0) {
				nosolve = true;
			} else if (strcmp(argv[i], "-lp") == 0) {
				if (++i < argc) {
					struct stat sts;
					if (stat(argv[i], &sts) == -1 && errno == ENOENT) {
						fprintf(stderr, "ERROR: -lp option require a lp solver: -lp <lpsolver> and %s does not exist.\n", argv[i]);
						exit(-1);
					} else
						solver = new_lp_solver(argv[i]);
				} else {
					fprintf(stderr, "ERROR: -lp option require a lp solver: -lp <lpsolver>\n");
					exit(-1);
				}
#ifdef USECPLEX
			} else if (strcmp(argv[i], "-cplex") == 0) {
				solver = new_cplex_solver();
#endif
#ifdef USELPSOLVE
			} else if (strcmp(argv[i], "-lpsolve") == 0) {
				solver = new_lpsolve_solver();
#endif
#ifdef USEGLPK
			} else if (strcmp(argv[i], "-glpk") == 0) {
				solver = new_glpk_solver(false);
#endif
			} else {
				fprintf(stderr, "ERROR: unrecognized option %s\n", argv[i]);
				exit(-1);
			}
		}
	}

	// if no input file defined, then use stdin
	if (! got_input) {
		switch (parse_pslp(cin)) {
		case 0: break;
		case 1: fprintf(stderr, "ERROR: invalid input in problem.\n"); exit(-1);
		case 2: fprintf(stderr, "ERROR:range(int min, int max) : min(min), max(max), max_limit(max) {} parser memory issue.\n"); exit(-1);
		}
	}
	//Generate problem instance
	the_problem->setSeed(*seed);
	the_problem->generateNetwork(HIERARCHIC);

	ostream& out = got_output ? output_file : cout;
	// if whished, print out the read problem
	if (verbosity >= VERBOSE) {
		print_generator_summary(out, the_problem);
		export_problem(the_problem);
	}
	if (verbosity >= DEFAULT) {
		print_problem(out, the_problem);
		out << "c " << *seed << " SEED" <<endl;
		out << "================================================================" << endl;
	}


	// choose the solver
	if (solver == (abstract_solver *)NULL)
#ifdef USECPLEX
		solver = new_cplex_solver();
#else
#ifdef USEGLPK
	solver = new_glpk_solver(false);
#else
#ifdef USELPSOLVE
	solver = new_lpsolve_solver();
#else
	{	fprintf(stderr, "ERROR: no solver defined\n"); exit(-1);}
#endif
#endif
#endif

	// check criteria properties
	//		for (vector<abstract_criteria *>::iterator icrit = criteria_with_property.begin(); icrit != criteria_with_property.end(); icrit++)
	//			(*icrit)->check_property(the_problem);

	// default combiner
	if (combiner == (abstract_combiner *)NULL) {
		CriteriaList *criteria = new CriteriaList();
		//criteria->push_back(new removed_criteria());
		combiner = new lexicographic_combiner(criteria);
	}

	problem = the_problem;
	// combiner initialization
	combiner->initialize(problem, solver);


	// generate the constraints, solve the problem and print out the solutions
	//if ((problem->all_packages->size() > 0) && (generate_constraints(problem, *solver, *combiner) == 0) && (! nosolve) && (solver->solve())) {



	int status = ERROR;
	if(generate_constraints(problem, *solver, *combiner) == 0) {
		if(nosolve) status = UNKNOWN;
		else status = solver->solve();
	}

	if(verbosity >= DEFAULT) {
		out << "================================================================" << endl;
		out << "c " << solver->objectiveCount() << " OBJECTIVES " << obj_descr << endl;
	}

	if(verbosity >= QUIET) {
		switch (status) {
		case UNKNOWN:
			out << "s UNKNOWN" << endl;
			break;
		case UNSAT:
			out << "s UNSAT" << endl;
			break;
		case SAT:
			out << "s SAT" << endl;
			break;
		case OPTIMUM:
			out << "s OPTIMUM_FOUND" << endl;
			break;
		default:
			out << "s ERROR" << endl;
			break;
		}
	}


	if(status == OPTIMUM || status == SAT) {
		solver->init_solutions();
		double obj = solver->objective_value();
		if(verbosity >= QUIET) {
			out << "o " << solver->objective_value() << endl;

		}
	}

	if(verbosity >= DEFAULT) {
		out << "d RUNTIME " << solver->timeCount() << endl;
		out << "d NODES " << solver->nodeCount() << endl;
		out << "d NBSOLS " << solver->solutionCount() << endl;
		if(status == OPTIMUM || status == SAT) {
			out << "d OBJECTIVE " << solver->objective_value() << endl; //For compatibility with grigrid scripts
			print_solution(out, the_problem, solver);
			print_messages(out, the_problem, solver);
			if(verbosity >= VERBOSE) {
				export_solution(the_problem, solver, obj_descr);
			}
		}
	}


	if (got_output) {
		output_file.close();
	}
	if(verbosity >= DEFAULT) {
		out << "================================================================" << endl << endl;
	}
	exit( status == ERROR ? 1 : 0);
}

int parse_pslp(istream& in)
{
	if(the_problem) delete the_problem;
	the_problem = new PSLProblem();
	in >> *the_problem;
	return 0;
}


void print_problem(ostream& out, PSLProblem *problem)
{
	out << "================================================================" << endl;
	out << "c " << problem->groupCount() << " GROUPS    "
			<< problem->facilityTypeCount() << " FTYPES    "
			<< problem->levelTypeCount() << " LEVELS    "
			<< endl;

	int clientCount = 0;
	int pservCount = 0;
	for(NodeIterator i = problem->nbegin() ; i!=  problem->nend() ; i++) {
		clientCount += i->getType()->getTotalDemand();
		pservCount += i->getType()->getTotalCapacity();
	}
	out << "c " << problem->nodeCount() <<" FACILITIES    "
			<< pservCount << " PSERVERS    "
			<< clientCount << " CLIENTS "
			<<endl ;
	if(problem->groupCount() > 1) {
		int demands[problem->groupCount()];
		for (int g = 0; g < problem->groupCount(); ++g) {
			demands[g] = 0;
		}
		for(NodeIterator i = problem->nbegin() ; i!=  problem->nend() ; i++) {
			for (int g = 0; g < problem->groupCount(); ++g) {
				demands[g] += i->getType()->getDemand(g);
			}
		}
		out << "c ";
		for (int g = 0; g < problem->groupCount(); ++g) {
			out << demands[g] << " ";
		}

		out << "DEMANDS" << endl;
	}
	if(problem->serverTypeCount() > 1) {
		int pserv[problem->serverTypeCount()];
		for (int k = 0; k < problem->serverTypeCount(); ++k) {
			pserv[k] = 0;
		}
		for(NodeIterator i = problem->nbegin() ; i!=  problem->nend() ; i++) {
			for (int k = 0; k < problem->serverTypeCount(); ++k) {
				pserv[k] += i->getType()->getServerCapacity(k);
			}
		}

		out << "c ";
		for (int k = 0; k < problem->serverTypeCount(); ++k) {
			out << pserv[k] << " ";
		}
		out << "PSERVERS" << endl;
	}
	if (verbosity >= ALL && problem->getRoot()) {
		out << endl;
		problem->getRoot()->print(out);
	}
}

extern void export_problem(PSLProblem *problem)
{
	inst2dotty(*problem);
}



extern void print_generator_summary(ostream & out, PSLProblem *problem)
{
	out << "================================================================" << endl;
	problem->print_generator(out);
	out << "================================================================" << endl;
	out << *problem;
}



void print_solution(ostream & out, PSLProblem *problem, abstract_solver *solver)
{
	int cpt = 0;
	out << "s";
	for(NodeIterator i = problem->nbegin() ; i!=  problem->nend() ; i++) {
		int servers = solver->get_solution(problem->rankX(*i));
		if(servers > 0) {
			out << ( ++cpt % 10 == 0 ? "\ns " : " ");
			//Print #pservers
			out << i->getID() << "[" << servers;
			//Print pservers capacity
			if(servers < i->getType()->getTotalCapacity()) {
				out << "/" << i->getType()->getTotalCapacity();
			}
			out << "]";
			//Print pservers by type
			if(problem->serverTypeCount() > 1) {
				out << "{";
				for (int k = 0; k < problem->serverTypeCount(); ++k) {
					if(k > 0) out << ",";
					out << solver->get_solution(problem->rankX(*i, k));
				}
				out << "}";
			}
		}
	}
	out << endl;
}



void export_solution(PSLProblem *problem, abstract_solver *solver,char* objective)
{
	solution2dotty(*problem, *solver, objective);
}



void print_messages(ostream & out, PSLProblem *problem, abstract_solver *solver)
{

	//Compute total pserver capacity
	double capa = 0;
	for(NodeIterator i = problem->nbegin() ; i!=  problem->nend() ; i++) {
		for (int k = 0; k < problem->serverTypeCount(); ++k) {
			capa += solver->get_solution(problem->rankX(*i, k)) * problem->getServer(k)->getMaxConnections();
		}
	}
	//Display pserver messages.
	CUDFcoefficient tot_facilities=0;
	CUDFcoefficient tot_pserv = 0;
	CUDFcoefficient tot_rel_pserv = 0;
	CUDFcoefficient pserv[problem->serverTypeCount()];
	for (int k = 0; k < problem->serverTypeCount(); ++k) {
		pserv[k]=0;
	}
	for(NodeIterator i = problem->nbegin() ; i!=  problem->nend() ; i++) {
		CUDFcoefficient _pserv = solver->get_solution(problem->rankX(*i));
		if(_pserv > 0) {
			tot_facilities++;
			tot_pserv += _pserv;
			if(i->isReliableFromRoot()) {
				tot_rel_pserv += _pserv;
			}
			for (int k = 0; k < problem->serverTypeCount(); ++k) {
				CUDFcoefficient _pservk = solver->get_solution(problem->rankX(*i, k));
				capa += _pservk * problem->getServer(k)->getMaxConnections();
				pserv[k] += _pservk;
			}
		}
	}
	out << "d FACILITIES " << tot_facilities << endl;
	out << "d PSERVERS " << tot_pserv << endl;
	if(problem->serverTypeCount() > 1) {
		out << "d VEC_PSERVERS ";
		for (int k = 0; k < problem->serverTypeCount(); ++k) {
			out << pserv[k] << " ";
		}
		out <<endl;
	}
	out << "d REL_PSERVERS " << tot_rel_pserv << endl;
	//Display spare capacity.
	double spare_capa[problem->stageCount()];
	double avg_spare_capa = 0;
	for (int s = 1; s < problem->stageCount(); ++s) {
		double clients = 0;
		for(NodeIterator i = problem->nbegin() ; i!=  problem->nend() ; i++) {
			clients += solver->get_solution(problem->rankY(*i, s));
		}
		spare_capa[s] = (capa-clients)/capa;
		avg_spare_capa +=spare_capa[s];
	}
	out.precision(3);
	avg_spare_capa/= problem->stageCount()-1;
	out << "d SPARE_CAPA " << fixed << avg_spare_capa <<endl;
	if(problem->stageCount() > 2) {
		out << "d VEC_SPARE_CAPA ";
		for (int s = 1; s < problem->stageCount(); ++s) {
			out << spare_capa[s] << " ";
		}
		out << endl;
	}
}






