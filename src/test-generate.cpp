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

using namespace std;


void print(FacilityNode* o, FacilityNode* d) {
	cout << *o << " -> " << *d << endl;
}


int main() {
	cout << "Hello World!!!" << endl; // prints Hello World!!!
	vector<FacilityType*> ftypes;
	//ROOT
	unsigned int s[] = {5,5};
	double b[] = {1, 0, 0, 0};
	ftypes.push_back(new FacilityType(0,10,s,1,1, b,1));
	//REGIONS
	unsigned int s1[] = {2,3};
	double b1[] = {0.6, 0.2,0.15,0.05};
	ftypes.push_back(new FacilityType(1,200,s1,5, 0.5, b1,0.6));
	unsigned int s2[] = {3,1};
	double b2[] = {0.7, 0.15,0.1,0.05};
	ftypes.push_back(new FacilityType(1,300,s2,8, 0.6, b2,0.8));
	//FINALS
	unsigned int s3[] = {1,1};
	double b3[] = {0.3, 0.3,0.2,0.2};
	ftypes.push_back(new FacilityType(2,50,s3,3, 0.5, b3,0.5));
	unsigned int s4[] = {0,1};
	double b4[] = {0.2, 0.3,0.3,0.2};
	ftypes.push_back(new FacilityType(2,20,s4,4, 0.6, b4,0.5));
	cout << *ftypes.front() << endl;
	for ( size_t i = 0; i < ftypes.size(); ++i ) {
		cout << *ftypes[i] << endl;
	}

	FacilityNode* root = generateSubtree(new FacilityNode(ftypes[0]), ftypes);
	root->printSubtree();
	root->toChild(0)->getDestination()->toChild(0)->forEachPath();
	root->toChild(0)->forEachPath(print);
	//ofstream myfile;
	//	myfile.open ("/tmp/pserver.dot");
	//	myfile << "digraph G {\n";
	//	root->toDotty(myfile);
	//	myfile << "}\n";
	//	myfile.close();
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
