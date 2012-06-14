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


//TODO Add test void forEachPath(FuncType functor) const;
PSLProblem* initProblem() {
	ifstream in;

	in.open("benchmarks/instances/sample-server.dat");
	if (!in) {
		cout << "Unable to open file";
		exit(1); // terminate with error
	}
	PSLProblem* problem = new PSLProblem();
	in >> *problem;
	in.close();

	problem->setSeed(SEED);
	problem->generateNetwork(true);
	//#ifndef NDEBUG //Mode Debug
	//	cout << *problem << endl << endl;
	//#endif
	return problem;

}

BOOST_AUTO_TEST_SUITE(Tests)

BOOST_AUTO_TEST_CASE(NetworkIterators)
{

	PSLProblem* problem = initProblem();
	/////////////////////////////////////////////////
	//		UnitTest LinkIterator
	////////////////////////////////////////////////

	LinkIterator itL = problem->getRoot()->lbegin();
	//Copy ctor
	//	
	LinkIterator itL_copy(itL);
	BOOST_CHECK(itL == itL_copy);

	//Operator =
	LinkIterator itL_copy2 = problem->getRoot()->lbegin();
	itL_copy2 = itL;
	BOOST_CHECK(itL == itL_copy2);

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
	BOOST_CHECK(count_links == problem->linkCount());


	/////////////////////////////////////////////////
	//		UnitTest NodeIterator
	////////////////////////////////////////////////

	NodeIterator itN = problem->getRoot()->nbegin();
	//Copy ctor
	//	
	NodeIterator itN_copy(itN);
	BOOST_CHECK(itN == itN_copy);

	//Operator =
	NodeIterator itN_copy2 = problem->getRoot()->nbegin();
	itN_copy2 = itN;
	BOOST_CHECK(itN == itN_copy2);

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
	BOOST_CHECK(count_nodes == problem->nodeCount());

	/////////////////////////////////////////////////
	//		UnitTest AncestorIterator
	////////////////////////////////////////////////

	AncestorIterator itA = problem->getRoot()->getChild(0)->getChild(0)->abegin();
	//Copy ctor
	//
	AncestorIterator itA_copy(itA);
	BOOST_CHECK(itA == itA_copy);

	//Operator =
	AncestorIterator itA_copy2 = problem->getRoot()->getChild(0)->getChild(0)->abegin();
	itA_copy2 = itA;
	BOOST_CHECK(itA == itA_copy2);

	//Operator ++
	//
	itA++;
	itA_copy++;
	BOOST_CHECK(itA == itA_copy);

	//Operator !=
	//
	BOOST_CHECK(itA != itA_copy2);
	BOOST_CHECK(itA_copy != itA_copy2);

	//Operator *
	//
	BOOST_CHECK(*itA == *itA_copy);
	BOOST_CHECK(*itA != *(itA_copy2));

	//Operator ->
	//
	BOOST_CHECK(itA->getID() == itA_copy->getID());

	//Count ancestors
	int count = 0;
	for( AncestorIterator i = problem->getRoot()->getChild(0)->getChild(0)->abegin();i != problem->getRoot()->getChild(0)->getChild(0)->aend();i++) {
		count++;
	}
	BOOST_CHECK(count == 2);

	//Count root ancestors
	count = 0;
	for( AncestorIterator i = problem->getRoot()->abegin();i != problem->getRoot()->aend();i++) {
		count++;
	}
	BOOST_CHECK(count == 0);


	/////////////////////////////////////////////////
	//		UnitTest PathIterator
	////////////////////////////////////////////////
	//Count paths
	count = 0;
	for( PathIterator i = problem->getRoot()->pbegin();i != problem->getRoot()->pend();i++) {
		//	cout << *(*i).first << " -> "<< *(*i).second << endl;
		count++;
	}
	BOOST_CHECK(count == problem->pathCount());
}

BOOST_AUTO_TEST_CASE(networkGeneration)
{

	PSLProblem* problem = initProblem();
	IntList level1(problem->getLevelNodeCounts());
	problem->setSeed(SEED);
	problem ->generateNetwork(true);
	//FIXME problem->generateNetwork(false);
	cout << *problem;
	IntList level2(problem->getLevelNodeCounts());
	BOOST_CHECK_EQUAL_COLLECTIONS(level1.begin(), level1.end(), level2.begin(), level2.end());
}

BOOST_AUTO_TEST_CASE(networkExample)
{
	PSLProblem* problem = initProblem();
	ofstream myfile;
	char* name = tmpnam(NULL);
	myfile.open(name);
	//myfile.open ("/tmp/pserver.dot");
	problem->toDotty(myfile);
	myfile.close();
	//Test Ranks
	problem->toRanks(cout);
	FacilityNode* n0 = problem->getRoot();
	FacilityNode* n3 = problem->getRoot()->getChild(0)->getChild(0);
	FacilityNode* n6 = problem->getRoot()->getChild(1)->getChild(0);
	//cout << *n3 << " " << *n6 << endl;
	BOOST_CHECK(problem->rankX(n3) == 3);
	BOOST_CHECK(problem->rankX(n6) == 6);
	BOOST_CHECK(problem->rankX(n3, 0) == 18);
	BOOST_CHECK(problem->rankX(n6, 0) == 21);
	BOOST_CHECK(problem->rankY(n3, 0) == 36);
	BOOST_CHECK(problem->rankY(n6, 1) == 43);
	BOOST_CHECK(problem->rankZ(n3, 0) == 66);
	BOOST_CHECK(problem->rankZ(n6, 1) == 73);
	BOOST_CHECK(problem->rankY(n3->toFather(), 0) == 94);
	BOOST_CHECK(problem->rankY(n6->toFather(), 1) == 101);
	BOOST_CHECK(problem->rankZ(n0, n3, 0) == 146);
	BOOST_CHECK(problem->rankZ(n0, n6, 1) == 153);
	BOOST_CHECK(problem->rankB(n0, n3, 0) == 214);
	BOOST_CHECK(problem->rankB(n0, n6, 1) == 221);



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
