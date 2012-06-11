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

#define INST "cplexpb"
#define FLOW "sol-flow-"
#define PATH "sol-path-"
#define DOT ".dot"

void stylePServers(ostream & out, const int servers) {
	switch (servers) {
	case 0:	out << ",style=dashed"; break;
	case 1:break;
	case 2: out << ",style=filled,fillcolor=lightgoldenrod";break;
	case 3: out << ",style=filled,fillcolor=palegreen";break;
	case 4: out << ",style=filled,fillcolor=lightseagreen";break;
	default:
		out << ",style=filled,fillcolor=salmon";break;
		break;
	}
}

void node2dotty(ostream & out, FacilityNode* i,PSLProblem & problem, abstract_solver & solver, unsigned int stage) {
	CUDFcoefficient demand = stage == 0 ?
			solver.get_solution(problem.rankX(i)) : i->getType()->getDemand(stage-1);
	CUDFcoefficient servers = solver.get_solution(problem.rankX(i));
	CUDFcoefficient connections = solver.get_solution(problem.rankY(i, stage));
	out << i->getID();
	out << "[shape=record, label=\"{{" << i->getID() << "}|";
	if(stage == 0 || servers == 0 ) {
		out << "{" << demand << "}";
	} else {
		out << "{" << demand << "|" << servers << "|" << connections << "}";
	}
	out << "}\"";
	stylePServers(out, servers);
	out << "];" << endl;
}

void colorConnection(ostream & out, const int connections) {
	if(connections >= 50) {
		if(connections >= 1000) {
			out << ",color=firebrick";
		} else if(connections >= 500) {
			out << ",color=salmon";
		} else if(connections >= 250) {

			out << ",color=midnightblue";
			//out << ",color=seagreen";
		}else if(connections >= 100) {
			out << ",color=seagreen";
		} else {
			out << ",color=darkgoldenrod";
		}
	}
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
			out << "[label=\"" << connections << "\"";
			colorConnection(out, connections);
			if (l->isReliable()) {
				out << ", style=bold ";
			}
			out <<"];" << endl;
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
		ss << FLOW << i << DOT;
		//cout << "## " << ss.str() << endl;
		myfile.open(ss.str().c_str());
		myfile << "digraph F" << i << "{" <<endl;
		flow2dotty(myfile, problem, solver, i);
		myfile << endl << "}" << endl;
		myfile.close();
	}
}


void colorUnitBandwidth(ostream & out, const double unitBandwidth) {
	if(unitBandwidth >= 10) {
		if(unitBandwidth >= 1000) {
			out << ",color=firebrick";
		} else if(unitBandwidth >= 250) {
			out << ",color=salmon";
		} else if(unitBandwidth >= 100) {

			out << ",color=midnightblue";
			//out << ",color=seagreen";
		}else if(unitBandwidth >= 50) {
			out << ",color=seagreen";
		} else {
			out << ",color=darkgoldenrod";
		}
	}
}

bool isReliablePath(const FacilityNode* origin, FacilityNode* destination) {
	while(destination != origin) {
		if(destination->toFather()->isReliable()) {
			destination = destination->getFather();
			if(!destination) {
				return false;
			}
		}else return false;
	}
	return true;
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
					double bandwidth = solver.get_solution(problem.rankB(*i,*j, stage));
					bandwidth/=connections;
					out.precision(1);
					out << i->getID() << " -> " << j->getID();
					out << "[label=\"" << connections <<
							"\\n" << scientific << bandwidth << "\"";
					colorUnitBandwidth(out, bandwidth);
					if (isReliablePath(*i, *j)) {
						out << ", style=bold ";
					}
					out << "];" << endl;
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
		ss << PATH << i << DOT;
		myfile.open (ss.str().c_str());
		myfile << "digraph P" << i << "{" <<endl;
		path2dotty(myfile, problem, solver, i);
		myfile << endl << "}" << endl;
		myfile.close();
	}
}


void inst2dotty(PSLProblem &problem) {
	ofstream myfile;
	myfile.open (INST DOT);
	problem.toDotty(myfile);
	myfile.close();
}

void solution2dotty(PSLProblem &problem, abstract_solver& solver) {
	flow2dotty(problem, solver);
	path2dotty(problem, solver);
}





