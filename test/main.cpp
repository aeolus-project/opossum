#define BOOST_TEST_DYN_LINK
#define BOOST_TEST_MODULE PulseTest

#include <boost/test/unit_test.hpp>
#include <exception>

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


BOOST_AUTO_TEST_CASE(networkGeneration)
{
	//FIXME Tests does not compile !?
		ifstream in;

		in.open("benchmarks/instances/sample-server.dat");
		if (!in) {
			BOOST_ERROR("Unable to open file");
		}
		PSLProblem* problem = new PSLProblem();
		in >> *problem;
	//	in.close();
	//		problem->generateNetwork();
	//		cout << *problem;
	//		ofstream myfile;
	//		myfile.open ("/tmp/pserver.dot");
	//		problem->toDotty(myfile);
	//		myfile.close();

	//	FILE * pFile;
	//	pFile = tmpfile ();
	//	// temporary file created. code here.
	//	fclose (pFile);
}


BOOST_AUTO_TEST_CASE(simplePass)
{
	FacilityType* ftype = new FacilityType();

	cout << "Seed: " << 1000 << endl;
	ftype->setSeed(1000);
	cout << "Nb Random Facilities: " << ftype->genRandd() << endl;

	cout << "Seed: " << 3000 << endl;
	ftype->setSeed(3000);
	cout << "Nb Random Facilities: " << ftype->genRandd() << endl;

	cout << "Seed: " << 1000 << endl;
	ftype->setSeed(1000);
	cout << "Nb Random Facilities: " << ftype->genRandd() << endl;

	delete ftype;
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
