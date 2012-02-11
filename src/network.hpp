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

#include <stdlib.h>
#include <iostream>
#include <fstream>
#include <iterator>
#include <algorithm>
#include <vector>
#include <queue>
#include <assert.h>
#include <boost/random/variate_generator.hpp>
#include <boost/generator_iterator.hpp>
#include <boost/random/mersenne_twister.hpp>
#include <boost/random.hpp>
#include <boost/random/uniform_01.hpp>
#include <boost/random/binomial_distribution.hpp>

using namespace boost::random;
using namespace std;

// class predeclarations
class ServerType;
class FacilityType;
class FacilityNode;
class NetworkLink;
class LinkIterator;
class NodeIterator;
class PSLProblem;

typedef vector<unsigned int> IntList;
typedef vector<unsigned int>::iterator IntListIterator;

typedef vector<ServerType*> ServerTypeList;
typedef vector<ServerType*>::iterator ServerTypeListIterator;

typedef vector<FacilityType*> FacilityTypeList;
typedef vector<FacilityType*>::iterator FacilityTypeListIterator;

typedef vector<FacilityNode*> FacilityList;
typedef vector<FacilityNode*>::iterator FacilityListIterator;

typedef vector<NetworkLink*> LinkList;
typedef vector<NetworkLink*>::iterator LinkListIterator;

//Functors and templates

// Foncteur servant à libérer un pointeur - applicable à n'importe quel type
struct Delete {
	template<class T> void operator ()(T*& p) const {
		delete p;
		p = NULL;
	}
};

template<typename T>
T& dereference(T* ptr) {
	return *ptr;
}

class ServerType {
public:
	ServerType() :
		capacity(0), cost(0) {
	}
	ServerType(unsigned int capacity, unsigned int cost) :
		capacity(capacity), cost(cost) {
	}
	virtual ~ServerType() {
	}
	;
	inline unsigned int getCost() const {
		return cost;
	}
	inline unsigned int getMaxConnections() const {
		return capacity;
	}

	friend istream& operator>>(istream& in, ServerType& f);
protected:
private:
	unsigned int capacity;
	unsigned int cost;

};

class FacilityType {

public:
	FacilityType() :
		level(0), binornd(NULL), reliabilityProbability(1) {
		binornd = new variate_generator<mt19937&, binomial_distribution<> >(
				fake_binornd);
	}
	virtual ~FacilityType() {
		delete binornd;
	}
	inline unsigned int getLevel() const {
		return level;
	}
	inline unsigned int getDemand(unsigned int stage) const {
		return demands[stage];
	}
	inline unsigned int getTotalDemand() {
		unsigned int sum = 0;
		for (IntListIterator j = demands.begin(); j != demands.end(); ++j)
			sum += *j;
		return sum;
	}
	inline unsigned int getServerCapacity(const unsigned int stype) const {
		return serverCapacities[stype];
	}
	inline unsigned int getTotalCapacity() {
		unsigned int sum = 0;
		for (IntListIterator j = serverCapacities.begin();
				j != serverCapacities.end(); ++j)
			sum += *j;
		return sum;
	}
	unsigned int getConnexionCapacity(const ServerTypeList* servers) const;
	//string toGEXF();
	unsigned int genRandomFacilities();
	unsigned int genRandomBandwidthIndex();
	unsigned int genRandomBandwidthIndex(unsigned int maxBandwidth);
	bool genRandomReliability();

	istream& read(istream& in, const PSLProblem& problem);
	friend ostream& operator<<(ostream& out, const FacilityType& f);
protected:
private:
	static mt19937 random_generator;
	static uniform_01<mt19937&, double> randd;
	static variate_generator<mt19937&, binomial_distribution<> > fake_binornd;

	unsigned int level;
	IntList demands;
	IntList serverCapacities;
	variate_generator<mt19937&, binomial_distribution<> >* binornd;
	vector<double> bandwidthProbabilities;
	double reliabilityProbability;

};

class FacilityNode {
	friend class NetworkLink;

public:
	FacilityNode(unsigned int id, FacilityType* type) :
		id(id), type(type), father(NULL) {
	}
	~FacilityNode();
	inline unsigned int getID() const {
		return id;
	}
	inline FacilityType* getType() const {
		return type;
	}
	inline NetworkLink* toFather() const {
		return father;
	}
	FacilityNode* getFather() const;
	inline unsigned int getChildrenCount() const {
		return children.size();
	}
	inline NetworkLink* toChild(unsigned int i) const {
		return children[i];
	}
	FacilityNode* getChild(unsigned int i) const;
	inline bool isRoot() const {
		return father == NULL;
	}
	inline bool isInternalNode() const {
		return !isRoot() && !isLeaf();
	}
	inline bool isLeaf() const {
		return children.empty();
	}
	bool isReliableFromRoot();
	unsigned int getMinIncomingConnections(ServerTypeList* servers);
	ostream& toDotty(ostream& out);
	ostream& toGEXF(ostream& out);
	void print(ostream& out);

	LinkListIterator cbegin() {
		return children.begin();
	}

	LinkListIterator cend() {
		return children.end();
	}

	NodeIterator nbegin();
	NodeIterator nend();

	LinkIterator lbegin();
	LinkIterator lend();
protected:
private:
	unsigned int id;
	FacilityType* type;
	NetworkLink* father;
	LinkList children;



};

class LinkIterator: public std::iterator<std::forward_iterator_tag, FacilityNode> {
public:
	LinkIterator(FacilityNode* p);
	~LinkIterator();

	// The assignment and relational operators are straightforward
	LinkIterator& operator=(const LinkIterator& other) {
		current = other.current;
		return (*this);
	}

	bool operator==(const LinkIterator& other) {
		return (current == other.current);
	}

	bool operator!=(const LinkIterator& other) {
		return (current != other.current);
	}

	LinkIterator& operator++();

	LinkIterator& operator++(int) {
		//FIXME why a local variable (compilation warning).
		//LinkIterator tmp(*this);
		++(*this);
		return *this;
		//return(tmp);

	}

	NetworkLink operator*();

	NetworkLink* operator->() {
		return *current;
	}

private:
	LinkListIterator current;
	LinkListIterator end;
	deque<FacilityNode*> queue;
};

class NodeIterator: public std::iterator<std::forward_iterator_tag, FacilityNode> {
public:
	NodeIterator(FacilityNode* p) :
		node(p), clink(NULL), elink(NULL) {
	}
	~NodeIterator() {
		delete node;
	}

	// The assignment and relational operators are straightforward
	NodeIterator& operator=(const NodeIterator& other) {
		node = other.node;
		return (*this);
	}

	bool operator==(const NodeIterator& other) {
		return (node == other.node);
	}

	bool operator!=(const NodeIterator& other) {
		return (node != other.node);
	}

	NodeIterator& operator++();
	NodeIterator& operator++(int) {
		//FIXME Iterateurs
		//Iterator tmp(*this);
		++(*this);
		return *this;
		//return(tmp);

	}

	FacilityNode operator*() {
		return (*node);
	}

	FacilityNode* operator->() {
		return node;
	}

private:
	FacilityNode* node;
	LinkIterator clink;
	LinkIterator elink;

};

class NetworkLink {

public:

	NetworkLink(unsigned int id, FacilityNode* father, FacilityNode* child,
			PSLProblem& problem, bool hierarchic);
	~NetworkLink() {
		//FIXME ~NetworkLink()
		//delete origin;
		//delete destination;
	}
	inline unsigned int getID() const {
		return id;
	}
	inline FacilityNode* getOrigin() const {
		return origin;
	}
	inline FacilityNode* getDestination() const {
		return destination;
	}
	inline bool isReliable() const {
		return reliable;
	}
	inline unsigned int getBandwidth() const {
		return bandwidth;
	}
	void forEachPath() const;
	void forEachPath(void(*ptr)(FacilityNode* n1, FacilityNode* n2)) const;
	ostream& toDotty(ostream& out);
	ostream& toGEXF(ostream& out);
protected:
private:
	unsigned int id;
	FacilityNode *origin;
	FacilityNode *destination;
	unsigned int bandwidth;
	bool reliable;
};

class RankMapper {


public:
	RankMapper(PSLProblem& problem);
	~RankMapper() {
	}
	int rankX(FacilityNode* node);
	int rankX(FacilityNode* node, unsigned int stype);
	int rankY(FacilityNode* node, unsigned int stage);
	int rankY(NetworkLink* link, unsigned int stage);
	int rankZ(FacilityNode* source, FacilityNode* destination,
			unsigned int stage);
	int rankB(FacilityNode* source, FacilityNode* destination,
			unsigned int stage);
private:
	int offsetXk();
	int offsetYi();
	int offsetYij();

	int rank(FacilityNode* source, FacilityNode* destination);
	int rank(FacilityNode* source, FacilityNode* destination,
			unsigned int stage);

	int offsetZ();
	int offsetB();

	inline int linkCount() {
		return nodeCount - 1;
	}
	inline int pathCount() {
		return 0;
	}
	unsigned int nodeCount;
	unsigned int stageCount;
	unsigned int serverCount;
	IntList levelNodeCounts;

};

class PSLProblem {

public:
	PSLProblem() :
		numberOfGroups(0), root(NULL), nodeCount(0) {
	}
	~PSLProblem() {
		delete root;
		for_each(servers.begin(), servers.end(), Delete());
		for_each(facilities.begin(), facilities.end(), Delete());
	}

	inline unsigned int getBandwidth(unsigned int idx) const {
		return bandwidths[idx];
	}
	inline unsigned int getNbBandwidths() const {
		return bandwidths.size();
	}
	inline unsigned int getNbServers() const {
		return servers.size();
	}
	inline unsigned int getNbGroups() const {
		return numberOfGroups;
	}
	inline unsigned int getNbFacilities() const {
		return facilities.size();
	}

	FacilityNode* generateNetwork();

	//generate Breadth-First Numbered Tree
	FacilityNode* generateNetwork(bool hierarchic);

	bool checkNetwork();
	inline FacilityNode* getRoot() const {
		return root;
	}
	inline unsigned int getNodeCount() const {
		return nodeCount;
	}

	inline IntList getLevelNodeCounts() const {
		return levelNodeCounts;
	}

	inline unsigned int getLinkCount() const {
		return nodeCount - 1;
	}

	ostream& toDotty(ostream& out);

	friend ostream& operator<<(ostream& out, const PSLProblem& f);
	friend istream& operator>>(istream& in, PSLProblem& problem);
protected:
private:
	IntList bandwidths;
	ServerTypeList servers;
	unsigned int numberOfGroups;
	FacilityTypeList facilities;
	FacilityNode* root;
	IntList levelNodeCounts;
	unsigned int nodeCount;


};

ostream& operator<<(ostream& out, const PSLProblem& f);
istream& operator>>(istream& in, PSLProblem& s);

ostream& operator<<(ostream& out, const ServerType& s);
istream& operator>>(istream& in, ServerType& s);

ostream& operator<<(ostream& out, const FacilityType& f);

ostream& operator<<(ostream& out, const FacilityNode& n);
ostream& operator<<(ostream& out, const NetworkLink& l);

#endif /* NETWORK_HPP_ */
