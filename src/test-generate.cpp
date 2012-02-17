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
 *      * Neither the name of the Arnaud Malapert nor the
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
 */
#include "network.hpp"
#include "GexfGen.hpp"

#include <string>
#include <sstream>
#include <vector>
#include <iterator>
#include <iostream>
#include <algorithm>

using namespace std;

void print(FacilityNode* o, FacilityNode* d) {
	cout << *o << " -> " << *d << endl;
}


int main() {
	cout << "Hello World!!!" << endl; // prints Hello World!!!
	ifstream in;

	in.open("benchmarks/instances/sample-server.dat");
	if (!in) {
		cout << "Unable to open file";
		exit(1); // terminate with error
	}
	PSLProblem* problem = new PSLProblem();
	in >> *problem;
	in.close();
	//	cout << *problem;
	//FacilityNode* root = new FacilityNode(problem->facilities[0]);
	//
	problem->setSeed(10);
	problem->generateNetwork(true);
	cout << *problem;
	/* refreshTree TESTED
	problem->generateNetwork(true);
	cout << *problem;
	*/
//
//	ofstream myfile;
//	myfile.open ("/tmp/pserver.dot");
//	problem->toDotty(myfile);
//	myfile.close();


//	for( LinkIterator i = problem->getRoot()->lbegin();i != problem->getRoot()->lend();i++) {
//		//cout << i->getID() << " ";
//		cout << *i << endl;
//	}
//	cout << endl;


	//Bad test
	//	cout << "Nodes :";
//	cout << endl;

	for( NodeIterator i = problem->getRoot()->nbegin();i != problem->getRoot()->nend();i++) {
		cout << i->getID() << endl;
	}
	cout << endl;
	//	cout << *problem;


//	cout << "ok: " << problem->checkNetwork() << endl;
//	cout << "hierarchic: " << problem->checkNetworkHierarchy() << endl;

//
//	RankMapper* rankM = new RankMapper((*problem));
//	cout << *rankM;
//	cout << rankM->rankX(problem->getRoot())<< endl;
//	FacilityNode* source = problem->getRoot()->getChild(1);
//	cout << rankM->rankX(problem->getRoot(), 1) << endl;
//	FacilityNode* destination = source->getChild(0)->getChild(0);
//	cout << rankM->rankZ(source, destination,1) << endl;



//	AbstractGexfGen* generatorGexf = new InstanceGexfGen(root);
//	generatorGexf->writeToFile("test.gexf");
//	delete generatorGexf;


	return 0;
}
