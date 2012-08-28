/*******************************************************/
/* oPoSSuM solver: graphviz.cpp                        */
/* methods of the graphviz interface    		       */
/* (c) Arnaud Malapert I3S (UNS-CNRS) 2012             */
/*******************************************************/
#ifndef GRAPHVIZ_HPP_
#define GRAPHVIZ_HPP_


#include <abstract_solver.h>

using namespace std;

extern void inst2dotty(PSLProblem &problem);

extern void pserv2dotty(PSLProblem &problem, abstract_solver& solver);

extern void flow2dotty(ostream& out, PSLProblem& problem, abstract_solver& solver, unsigned int stage);

extern void path2dotty(ostream& out, PSLProblem &problem, abstract_solver& solver, unsigned int stage);

extern void flow2dotty(PSLProblem &problem, abstract_solver& solver, char* title);

extern void path2dotty(PSLProblem &problem, abstract_solver& solver, char* title);

extern void solution2dotty(PSLProblem &problem, abstract_solver& solver, char* title);


#endif /* GRAPHVIZ_HPP_ */
