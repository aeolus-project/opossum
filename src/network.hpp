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
#ifndef NETWORK_HPP_
#define NETWORK_HPP_

#include <iostream>
#include <fstream>
#include <iterator>
#include <algorithm>
#include <vector>
#include <cassert>
#include <boost/random/variate_generator.hpp>
#include <boost/generator_iterator.hpp>
#include <boost/random/mersenne_twister.hpp>
#include <boost/random.hpp>
#include <boost/random/uniform_01.hpp>
#include <boost/random/binomial_distribution.hpp>

//Define the seed of random
#define SEED 1000

//
//#include <boost/math/distributions/binomial.hpp>
//using boost::math::binomial;

using namespace boost::random;
using namespace std;

// Foncteur servant à libérer un pointeur - applicable à n'importe quel type
struct Delete
{
	template <class T> void operator ()(T*& p) const
	{
		delete p;
		p = NULL;
	}
};

template <typename T>
T& dereference(T* ptr) { return *ptr; }

class ServerType
{
public:
	ServerType() : capacity(0), cost(0) {
	}
	ServerType(unsigned int capacity, unsigned int cost) : capacity(capacity), cost(cost) {
	}
	virtual ~ServerType() {};
	inline unsigned int getCost() const { return cost; }
	inline unsigned int getMaxConnections() const { return capacity; }

	friend istream& operator>>(istream& in, ServerType& f);
protected:
private:
	unsigned int capacity;
	unsigned int cost;

};



class PSLProblem;

class FacilityType
{

public:
	FacilityType() : level(0), demand(0), binornd(NULL), reliabilityProbability(1),
					random_generator(default_random_generator), randd(NULL)
	{
		randd = new uniform_01< mt19937&, double >(random_generator);
		binornd = new variate_generator<mt19937&, binomial_distribution<> >(fake_binornd);
	}
	virtual ~FacilityType() {
		delete randd;
		delete binornd;
	}

	inline double genRandd() {
		return (*randd)();
	}

	inline unsigned int getLevel() const { return level; }
	inline unsigned int getDemand() const { return demand; }
	inline unsigned int getServerCapacity(const unsigned int stype) const {return serverCapacities[stype];
	}
	unsigned int getConnexionCapacity(const vector<ServerType*>* servers) const;
	inline unsigned int getMaxServerCapacities() const {
		int maxCap = 0;
		for(int i = 0; i < serverCapacities.size(); i++) {
			maxCap += serverCapacities[i];
		}
		return maxCap;
	}
	unsigned int genRandomFacilities();
	unsigned int genRandomBandwidthIndex();
	unsigned int genRandomBandwidthIndex(unsigned int maxBandwidth);
	bool genRandomReliability();

	void setSeed(int seed);

	istream& read(istream& in, const PSLProblem& problem);
	friend ostream& operator<<(ostream& out, const FacilityType& f);
protected:
private:
	static mt19937 default_random_generator;
	static variate_generator<mt19937&, binomial_distribution<> > fake_binornd;

	mt19937 random_generator;
	variate_generator<mt19937&, binomial_distribution<> >* binornd;
	uniform_01< mt19937&, double >* randd;

	unsigned int level;
	unsigned int demand;
	vector<unsigned int> serverCapacities;
	vector<double> bandwidthProbabilities;
	double reliabilityProbability;

};



class NetworkLink;


class FacilityNode
{
	friend class NetworkLink;
	static unsigned int NEXT_ID;

public:
	FacilityNode(FacilityType* type) : id(NEXT_ID++), type(type), father(NULL) {
	}
	~FacilityNode();
	inline unsigned int getID() const { return id; }
	inline FacilityType* getType() const { return type; }
	inline NetworkLink* toFather() const { return father; }
	FacilityNode* getFather() const;
	inline unsigned int getChildrenCount() const {return children.size();}
	inline NetworkLink* toChild(unsigned int i) const { return children[i]; }
	inline FacilityNode* getChild(unsigned int i) const;
	inline bool isRoot() const { return father == NULL; }
	inline bool isInternalNode() const {
		return !isRoot() && !isLeaf();
	}
	inline bool isLeaf() const {
		return children.empty();
	}
	unsigned int getMinIncomingConnections(vector<ServerType*>* servers);
	ostream& toDotty(ostream& out);
	void printSubtree();
protected:
private:
	unsigned int id;
	FacilityType* type;
	NetworkLink* father;
	vector<NetworkLink*> children;


};


class PSLProblem {


public:

	PSLProblem() {
	}
	~PSLProblem() {
		for_each(servers.begin(), servers.end(), Delete());
		for_each(facilities.begin(), facilities.end(), Delete());
	}
	inline unsigned int getBandwidth(unsigned int idx) const { return bandwidths[idx];}
	inline unsigned int getNbBandwidths() const { return bandwidths.size();}
	inline unsigned int getNbServers() const { return servers.size();}
	inline unsigned int getNbFacilities() const { return facilities.size();}

	FacilityNode* generateNetwork();

	friend ostream& operator<<(ostream& out, const PSLProblem& f);
	friend istream& operator>>(istream& in, PSLProblem& problem);
protected:
	FacilityNode* generateSubtree(FacilityNode* root);
private:
	vector<unsigned int> bandwidths;
	vector<ServerType*> servers;
	vector<FacilityType*> facilities;

};

class NetworkLink {

public:
	NetworkLink(FacilityNode* father, FacilityNode* child, vector<int>* bandwidths) : origin(father), destination(child), bandwidth(0),reliable(0)
	{
		father->children.push_back(this);
		child->father=this;
		bandwidth = (*bandwidths)[child->getType()->genRandomBandwidthIndex()];
		reliable = child->getType()->genRandomReliability();
	}

	NetworkLink(FacilityNode* father, FacilityNode* child, PSLProblem& problem, bool ignoreHierarchy) : origin(father), destination(child), bandwidth(0),reliable(0)
	{
		father->children.push_back(this);
		child->father=this;
		if( father->isRoot() || ignoreHierarchy) {
			bandwidth = problem.getBandwidth(child->getType()->genRandomBandwidthIndex());
			reliable = child->getType()->genRandomReliability();
		} else {
			unsigned int fbandw = father->toFather()->getBandwidth();
			int maxIndex = problem.getNbBandwidths();
			while( maxIndex >= 0 && problem.getBandwidth(maxIndex) > fbandw) {maxIndex--;}
			bandwidth = problem.getBandwidth(child->getType()->genRandomBandwidthIndex(maxIndex));
			reliable = father->toFather()->isReliable() && child->getType()->genRandomReliability();
		}
	}

	~NetworkLink() {
		delete origin;
		delete destination;
	}
	inline FacilityNode* getOrigin() const { return origin; }
	inline FacilityNode* getDestination() const { return destination; }
	inline bool isReliable() const { return reliable; }
	inline unsigned int getBandwidth() const { return bandwidth; }
	void forEachPath() const;
	void forEachPath(void (*ptr)(FacilityNode* n1, FacilityNode* n2)) const;
	ostream& toDotty(ostream& out);
protected:
private:
	FacilityNode *origin;
	FacilityNode *destination;
	unsigned int bandwidth;
	bool reliable;
};





ostream& operator<<(ostream& out, const PSLProblem& f);
istream& operator>>(istream& in, PSLProblem& s);

ostream& operator<<(ostream& out, const ServerType& s);
istream& operator>>(istream& in, ServerType& s);

ostream& operator<<(ostream& out, const FacilityType& f);


ostream& operator<<(ostream& out, const FacilityNode& n);
ostream& operator<<(ostream& out, const NetworkLink& l);


#endif /* NETWORK_HPP_ */
