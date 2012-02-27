/**
 *  Copyright (c) 2011, Arnaud Malapert and Mohamed Rezgui
 *  All rights reserved.
 *  Redistribution and use in source and binary forms, with or without
 *  modification, are permitted provided that the following conditions are met:
 *
 *      * Redistributions of source code must retain the above copyright
 *        notice, this list of conditions and the following disclaimer.
 *      * Redistributions in binary form must reproduce the above copyright
 *        notice, this list of conditions and the following disclaimer in the
 *        documentation and/or other materials provided with the distribution.
 *      * Neither the name of Arnaud Malapert nor the
 *        names of its contributors may be used to endorse or promote products
 *        derived from this software without specific prior written permission.
 *
 *  THIS SOFTWARE IS PROVIDED BY THE REGENTS AND CONTRIBUTORS ``AS IS'' AND ANY
 *  EXPRESS OR IMPLIED WARRANTIES, INCLUDING, BUT NOT LIMITED TO, THE IMPLIED
 *  WARRANTIES OF MERCHANTABILITY AND FITNESS FOR A PARTICULAR PURPOSE ARE
 *  DISCLAIMED. IN NO EVENT SHALL THE REGENTS AND CONTRIBUTORS BE LIABLE FOR ANY
 *  DIRECT, INDIRECT, INCIDENTAL, SPECIAL, EXEMPLARY, OR CONSEQUENTIAL DAMAGES
 *  (INCLUDING, BUT NOT LIMITED TO, PROCUREMENT OF SUBSTITUTE GOODS OR SERVICES;
 *  LOSS OF USE, DATA, OR PROFITS; OR BUSINESS INTERRUPTION) HOWEVER CAUSED AND
 *  ON ANY THEORY OF LIABILITY, WHETHER IN CONTRACT, STRICT LIABILITY, OR TORT
 *  (INCLUDING NEGLIGENCE OR OTHERWISE) ARISING IN ANY WAY OUT OF THE USE OF THIS
 *  SOFTWARE, EVEN IF ADVISED OF THE POSSIBILITY OF SUCH DAMAGE.
 *
 * 
 *  Created on: 27 févr. 2012
 */

#include "graphviz.hpp"

void node2dotty(ostream & out, FacilityNode* i,PSLProblem & problem, abstract_solver & solver, unsigned int stage) {
	CUDFcoefficient demand = stage == 0 ?
			solver.get_solution(problem.rankX(i)) : i->getType()->getDemand(stage-1);
	CUDFcoefficient provide = solver.get_solution(problem.rankY(i, stage));
	out << i->getID();
	out << "[shape=record,";
	if(provide>0) {
		out << "style=filled,";
	}
	out << "label=\"{{" << i->getID() << "}|{"
			<< demand << "|"
			<< provide << "}}\"";

	out << "];" << endl;
}

void flow2dotty(ostream & out, PSLProblem & problem, abstract_solver & solver, unsigned int stage)
{
	for(NodeIterator i = problem.nbegin() ; i!=  problem.nend() ; i++) {
		node2dotty(out, *i, problem, solver, stage);
	}
	for(LinkIterator l = problem.lbegin() ; l!=  problem.lend() ; l++) {
		CUDFcoefficient connections = solver.get_solution(problem.rankY(*l, stage));
		out << l->getOrigin()->getID() << " -> " << l->getDestination()->getID();
		if(connections > 0) {
			out << "[label=\"" << connections << "\"];\n";
		} else {
			out << "[style=\"invis\"];\n";
		}
	}
}



void flow2dotty(PSLProblem & problem, abstract_solver & solver)
{
	for (int i = 0; i < problem.stageCount(); ++i) {
		ofstream myfile;
		stringstream ss (stringstream::in | stringstream::out);
		ss << "flow-sol-" << i << ".dot";
		//cout << "## " << ss.str() << endl;
		myfile.open(ss.str().c_str());
		myfile << "digraph F" << i << "{" <<endl;
		flow2dotty(myfile, problem, solver, i);
		myfile << endl << "}" << endl;
		myfile.close();
	}
}



void path2dotty(ostream& out, PSLProblem & problem, abstract_solver & solver, unsigned int stage)
{

	for(NodeIterator i = problem.nbegin() ; i!=  problem.nend() ; i++) {
		node2dotty(out, *i, problem, solver, stage);
		if( ! i->isLeaf()) {
			NodeIterator j = i->nbegin();
			j++;
			while(j !=  i->nend()) {
				CUDFcoefficient connections = solver.get_solution(problem.rankZ(*i,*j, stage));
				if(connections > 0) {
					CUDFcoefficient bandwidth = solver.get_solution(problem.rankB(*i,*j, stage));
					out << i->getID() << " -> " << j->getID();
					out << "[label=\"" << connections <<
							"|" << bandwidth << "\"];" << endl;
				}
				j++;
			}
		}
	}
	//Add invisible arcs of the tree if needed
	for(LinkIterator l = problem.lbegin() ; l!=  problem.lend() ; l++) {
		CUDFcoefficient connections = solver.get_solution(problem.rankZ(l->getOrigin(), l->getDestination(), stage));
		if(connections == 0) {
			out << l->getOrigin()->getID() << " -> " << l->getDestination()->getID();
			out << "[style=\"invis\"];" <<endl;
		}
	}

}



void path2dotty(PSLProblem & problem, abstract_solver & solver)
{
	for (int i = 1; i < problem.stageCount(); ++i) {
		ofstream myfile;
		stringstream ss (stringstream::in | stringstream::out);
		ss << "path-sol-" << i << ".dot";
		myfile.open (ss.str().c_str());
		myfile << "digraph P" << i << "{" <<endl;
		path2dotty(myfile, problem, solver, i);
		myfile << endl << "}" << endl;
		myfile.close();
	}
}


void inst2dotty(PSLProblem &problem) {
	ofstream myfile;
	myfile.open ("instance.dot");
	problem.toDotty(myfile);
	myfile.close();
}

void solution2dotty(PSLProblem &problem, abstract_solver& solver) {
	flow2dotty(problem, solver);
	path2dotty(problem, solver);
}




