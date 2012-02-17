#define BOOST_TEST_DYN_LINK
#define BOOST_TEST_MODULE PulseTest

#include <boost/test/unit_test.hpp>
#include <exception>

#include <stdio.h>
#include <string>
#include <sstream>
#include <vector>
#include <iterator>
#include <iostream>
#include <algorithm>

#include "../src/network.hpp"
#include "../src/GexfGen.hpp"

#include "../src/network.cpp"
#include "../src/GexfGen.cpp"


int add(int i, int j)
{
	return i + j;
}

BOOST_AUTO_TEST_SUITE(Tests)

BOOST_AUTO_TEST_CASE(NetworkIterators)
{
	ifstream in;

	in.open("benchmarks/instances/sample-server.dat");
	if (!in) {
		cout << "Unable to open file";
		exit(1); // terminate with error
	}
	PSLProblem* problem = new PSLProblem();
	in >> *problem;
	in.close();
	
	problem->setSeed(10);
	problem->generateNetwork(true);
	cout << *problem;
	
	/////////////////////////////////////////////////
	//		UnitTest LinkIterator
	////////////////////////////////////////////////

	LinkIterator itL = problem->getRoot()->lbegin();
	//Copy ctor
	//	
	LinkIterator itL_copy(itL);
	BOOST_CHECK(itL == itL_copy);
	
	//Operator ++
	//	
	itL++;
	itL_copy++;
	BOOST_CHECK(itL == itL_copy);
	
	//Operator !=	
	//	
	BOOST_CHECK(itL != problem->getRoot()->lbegin());
	BOOST_CHECK(itL_copy != problem->getRoot()->lbegin());
	
	//Operator *
	//
	BOOST_CHECK(*itL == *itL_copy);
	BOOST_CHECK(*itL != *(problem->getRoot()->lbegin()));

	//Operator ->
	//
	BOOST_CHECK(itL->getID() == itL_copy->getID());
	BOOST_CHECK(itL->getID() != problem->getRoot()->lbegin()->getID());
	
	//Count Links
	//	
	int count_links = 0;
	for( LinkIterator i = problem->getRoot()->lbegin();i != problem->getRoot()->lend();i++) {
		count_links++;	
	}
	BOOST_CHECK(count_links == problem->getLinkCount());

	
	/////////////////////////////////////////////////
	//		UnitTest NodeIterator
	////////////////////////////////////////////////

	NodeIterator itN = problem->getRoot()->nbegin();
	//Copy ctor
	//	
	NodeIterator itN_copy(itN);
	BOOST_CHECK(itN == itN_copy);
	
	//Operator ++
	//	
	itN++;
	itN_copy++;
	BOOST_CHECK(itN == itN_copy);
	
	//Operator !=	
	//	
	BOOST_CHECK(itN != problem->getRoot()->nbegin());
	BOOST_CHECK(itN_copy != problem->getRoot()->nbegin());
	
	//Operator *
	//
	BOOST_CHECK(*itN == *itN_copy);
	BOOST_CHECK(*itN != *(problem->getRoot()->nbegin()));

	//Operator ->
	//
	BOOST_CHECK(itN->getID() == itN_copy->getID());
	BOOST_CHECK(itN->getID() != problem->getRoot()->nbegin()->getID());
	
	//Count nodes
	//	
	int count_nodes = 0;
	for( NodeIterator i = problem->getRoot()->nbegin();i != problem->getRoot()->nend();i++) {
		count_nodes++;
	}
	BOOST_CHECK(count_nodes == problem->getNodeCount());

}

BOOST_AUTO_TEST_CASE(networkGeneration)
{
	ifstream in;

	in.open("benchmarks/instances/sample-server.dat");
	if (!in) {
		BOOST_ERROR("Unable to open file");
	}
	PSLProblem* problem = new PSLProblem();
	in >> *problem;
	in.close();

	IntList level1, level2; //,*level2;

	for (int i = 0; i < 2; ++i) {
		problem->setSeed(10);
		problem->generateNetwork(true);
		//FIXME problem->generateNetwork(false);
		//Networks should only differ by their link attributes.
#ifndef NDEBUG //Mode Debug
		cout << *problem << endl << endl;
#endif
		BOOST_CHECK(problem->checkNetwork());
		BOOST_CHECK(problem->checkNetworkHierarchy());
		if(i==0) {
			IntList level1(problem->getLevelNodeCounts());
		} else {
			IntList level2(problem->getLevelNodeCounts());
		}
	}
	BOOST_CHECK(level1.size() == level2.size());
	for (int i = 0; i < level1.size(); ++i) {
		BOOST_CHECK(level1[i] == level2[i]);
	}

}

BOOST_AUTO_TEST_CASE(networkExample)
{
	ifstream in;

	in.open("benchmarks/instances/sample-server.dat");
	if (!in) {
		BOOST_ERROR("Unable to open file");
	}
	PSLProblem* problem = new PSLProblem();
	in >> *problem;
	in.close();
	problem->setSeed(2000);
	problem->generateNetwork(true);
#ifndef NDEBUG //Mode Debug
	cout << *problem << endl << endl;
#endif
	BOOST_CHECK(problem->checkNetwork());
	BOOST_CHECK(problem->checkNetworkHierarchy());

	ofstream myfile;
	char* name = tmpnam(NULL);
	myfile.open (name);
	//myfile.open ("/tmp/pserver.dot");
	problem->toDotty(myfile);
	myfile.close();


}


BOOST_AUTO_TEST_CASE(simplePass)
{

}
/*
BOOST_AUTO_TEST_CASE(TestGexf)
{
	//cout << "Hello World!!!" << endl; // prints Hello World!!!
	//srand(1000);
	int sum = 0;
	ifstream in;

	//FacilityType::setSeed(1000);

	in.open("benchmarks/instances/sample-server.dat");
	if (!in) {
		//exit(1); // terminate with error
		BOOST_ERROR("Error : Enable Unable to open file");
	}
	PSLProblem* problem = new PSLProblem();
	in >> *problem;
	in.close();
	cout << "Sum = " << sum << endl;
	cout << *problem;

	//FacilityNode* root = new FacilityNode(problem->facilities[0]);
	//
	FacilityNode* root = problem->generateNetwork();
	root->printSubtree();
	//	generateSubtree(root, *problem);
	ofstream myfile;
	myfile.open ("testFig1.dot");
	myfile << "digraph G {\n";
	root->toDotty(myfile);
	myfile << "}\n";
	myfile.close();

	AbstractGexfGen* generatorGexf = new InstanceGexfGen(root);
	generatorGexf->writeToFile("testFig1.gexf");
	delete generatorGexf;

	generatorGexf = new FlowConnectionsGexfGen(root);
	generatorGexf->writeToFile("testFig2.gexf");
	delete generatorGexf;

}
 */
/*
BOOST_AUTO_TEST_CASE(checkFailure)
{
	BOOST_CHECK(add(2, 2) == 5);
}

BOOST_AUTO_TEST_CASE(multipleCheckFailures)
{
	BOOST_CHECK(add(2, 2) == 1);
	BOOST_CHECK(add(2, 2) == 2);
	BOOST_CHECK(add(2, 2) == 3);
}

BOOST_AUTO_TEST_CASE(requireFailure)
{
	BOOST_REQUIRE(add(2, 2) == 5);
}

BOOST_AUTO_TEST_CASE(explicitError)
{
	BOOST_ERROR("Error message");
}

BOOST_AUTO_TEST_CASE(explicitFailure)
{
	BOOST_FAIL("Failure message");
}

BOOST_AUTO_TEST_CASE(errorThenFailure)
{
	BOOST_FAIL("Error message");
	BOOST_FAIL("Failure message");
}

BOOST_AUTO_TEST_CASE(uncaughtException)
{
	throw "Catch me if you can!";
}

BOOST_AUTO_TEST_CASE(stdException)
{
	throw new std::exception();
}

BOOST_AUTO_TEST_CASE(checkMessageFailure)
{
	BOOST_CHECK_MESSAGE(add(2, 2) == 5, "add(..) result: " << add(2, 2));
}

BOOST_AUTO_TEST_CASE(checkEqualFailure)
{
	BOOST_CHECK_EQUAL(add( 2,2 ), 5);
}

BOOST_AUTO_TEST_CASE(threeSeconds)
{
	sleep(3);
}

BOOST_AUTO_TEST_SUITE_END()
BOOST_AUTO_TEST_SUITE(PassingSuite)

BOOST_AUTO_TEST_CASE(pass1)
{
}

BOOST_AUTO_TEST_CASE(pass2)
{
}

BOOST_AUTO_TEST_CASE(pass3)
{
}
 */
BOOST_AUTO_TEST_SUITE_END()
