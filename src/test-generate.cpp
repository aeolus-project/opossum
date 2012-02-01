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
#include "./network.hpp"
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
	//	FacilityNode* root = generateSubtree(new FacilityNode(ftypes[0]), ftypes);
	//	root->printSubtree();
	//	root->toChild(0)->getDestination()->toChild(0)->forEachPath();
	//	root->toChild(0)->forEachPath(print);
	//
	//	string str = "Hello   How Are  You  Today";
	//	vector<string> v;
	//	istringstream buf(str);
	//	for(std::string token; getline(buf, token, ' '); ) {
	//		if(! token.empty()) v.push_back(token);
	//	}
	//
	//	copy(v.begin(), v.end(), std::ostream_iterator<std::string>(std::cout, "."));
	//	std::cout << '\n';
	//srand(1000);
	int sum = 0;
	ifstream in;

	in.open("benchmarks/instances/sample-server.dat");
	if (!in) {
		cout << "Unable to open file";
		exit(1); // terminate with error
	}
	PSLProblem* problem = new PSLProblem();
	in >> *problem;
	in.close();
	cout << "Sum = " << sum << endl;
	cout << *problem;
	FacilityNode* root = new FacilityNode(problem->facilities[0]);
	generateSubtree(root, *problem);
	ofstream myfile;
	myfile.open ("/tmp/pserver.dot");
	myfile << "digraph G {\n";
	root->toDotty(myfile);
	myfile << "}\n";
	myfile.close();
	//root->toDotty(cout);
	//	FacilityType* f1 = new FacilityType(1,100,a,10, 0.5, b,0.5);
	//	FacilityNode* n1 = new FacilityNode(f1);
	//	FacilityNode* n2 = new FacilityNode(f1);
	//	cout << *f1 << endl << *serverTypes[0] <<endl;
	//	for (int var = 0; var < 10; ++var) {
	//
	//		NetworkLink* l1 = new NetworkLink(n1, n2);
	//		cout << *l1 << endl;
	//	}
	//	cout << endl;
	//	bool v1 = true;
	//	bool v2 = false;
	//	v1 &= false;
	//	v2 &= false;
	//	cout << v1 << endl;
	//	cout << v2 << endl;
	//	v1 = true;
	//	v2 = false;
	//	v1 &= true;
	//	v2 &= true;
	//	cout << v1 << endl;
	//	cout << v2 << endl;
	return 0;
}
