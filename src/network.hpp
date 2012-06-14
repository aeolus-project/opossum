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
#include <cassert>
#include <boost/random/variate_generator.hpp>
#include <boost/generator_iterator.hpp>
#include <boost/random/mersenne_twister.hpp>
#include <boost/random.hpp>
#include <boost/random/uniform_01.hpp>
#include <boost/random/binomial_distribution.hpp>

#include "cudf_types.h"

//Define the seed of random
#define SEED 1000

using namespace boost::random;
using namespace std;

// class predeclarations
class ServerType;
class FacilityType;
class FacilityNode;
class NetworkLink;
class LinkIterator;
class NodeIterator;
class AncestorIterator;
class PathIterator;
class PSLProblem;

typedef vector<unsigned int> IntList;
typedef vector<unsigned int>::iterator IntListIterator;

typedef vector<CUDFcoefficient> CUDFcoefficientList;
typedef vector<CUDFcoefficient>::iterator CUDFcoefficientListIterator;

typedef vector<ServerType*> ServerTypeList;
typedef vector<ServerType*>::iterator ServerTypeListIterator;

typedef vector<FacilityType*> FacilityTypeList;
typedef vector<FacilityType*>::iterator FacilityTypeListIterator;

typedef vector<FacilityNode*> FacilityList;
typedef vector<FacilityNode*>::iterator FacilityListIterator;

typedef vector<NetworkLink*> LinkList;
typedef vector<NetworkLink*>::iterator LinkListIterator;

//Functors and templates

//Fonctor to delete an object in any type
struct FonctorDeletePtr {
	template<class T> void operator ()(T* p) const {
		delete p;
		p = NULL;
	}
};


template<typename T>
T& dereference(T* ptr) {
	return *ptr;
}

//---------------------------------------- 
//	ServerType Declaration
//----------------------------------------

//TODO add costs as parameters to criteria and replace server types by a coefficients list;

class ServerType {
public:
	ServerType() : capacity(0), cost(0) {}
	ServerType(unsigned int capacity, unsigned int cost) : capacity(capacity), cost(cost) {}

	virtual ~ServerType() {}

	inline CUDFcoefficient getCost() const {
		return cost;
	}
	inline CUDFcoefficient getMaxConnections() const {
		return capacity;
	}

	friend istream& operator>>(istream& in, ServerType& f);

private:
	CUDFcoefficient capacity;
	CUDFcoefficient cost;

};

//---------------------------------------- 
//	FacilityType Declaration
//----------------------------------------

class FacilityType {
public:
	FacilityType() : level(0), binornd(NULL), reliabilityProbability(1) {
		binornd = new variate_generator<mt19937&, binomial_distribution<> >(fake_binornd);
	}

	//Destructor of FacilityType
	//Delete the generator of binomial distribution
	//	
	virtual ~FacilityType() {
		delete binornd;
	}

	static void setStaticSeed(const unsigned int seed);
	void setSeed(const unsigned int seed);
	void setBinomial(unsigned int n, double p);

	inline unsigned int getLevel() const {
		return level;
	}
	inline CUDFcoefficient getDemand(unsigned int stage) const {
		return demands[stage];
	}
	inline CUDFcoefficient getTotalDemand() {
		CUDFcoefficient sum = 0;
		for (CUDFcoefficientListIterator j = demands.begin(); j != demands.end(); ++j)
			sum += *j;
		return sum;
	}
	inline CUDFcoefficient getServerCapacity(const unsigned int stype) const {
		return serverCapacities[stype];
	}

	inline CUDFcoefficient getTotalCapacity() {
		CUDFcoefficient sum = 0;
		for (CUDFcoefficientListIterator j = serverCapacities.begin(); j != serverCapacities.end(); ++j)
			sum += *j;
		return sum;
	}


	unsigned int genRandomFacilities();
	unsigned int genRandomBandwidthIndex();
	unsigned int genRandomBandwidthIndex(unsigned int maxBandwidth);
	bool genRandomReliability();


	istream& read(istream& in, const PSLProblem& problem);
	friend ostream& operator<<(ostream& out, const FacilityType& f);

private:
	static mt19937 default_random_generator;
	static uniform_01< mt19937&, double > randd;
	static variate_generator<mt19937&, binomial_distribution<> > fake_binornd;

	unsigned int level;
	CUDFcoefficientList demands;
	CUDFcoefficientList serverCapacities;
	vector<double> bandwidthProbabilities;
	double reliabilityProbability;
	variate_generator<mt19937&, binomial_distribution<> >* binornd;

};

//---------------------------------------- 
//	FacilityNode Declaration
//----------------------------------------

class FacilityNode {
	friend class NetworkLink;

public:
	FacilityNode(unsigned int id, FacilityType* type) : id(id), type(type), father(NULL) {
	}

	//Destructor of FacilityNode
	//Do not delete type and father
	//Delete all children
	//	
	~FacilityNode() {
		for_each(children.begin(), children.end(), FonctorDeletePtr());
	}

	//the keyword inline is used only on header file
	//
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
	ostream& toDotty(ostream& out);
	ostream& toGEXF(ostream& out);
	void print(ostream& out);

	//For LinkListIterator
	LinkListIterator cbegin() {
		return children.begin();
	}

	LinkListIterator cend() {
		return children.end();
	}

	//For NodeIterator
	NodeIterator nbegin();
	NodeIterator nend();

	//For LinkIterator
	LinkIterator lbegin();
	LinkIterator lend();

	//For AncestorIterator
	AncestorIterator abegin();
	AncestorIterator aend();

	//For PathIterator
	PathIterator pbegin();
	PathIterator pend();

private:
	unsigned int id;
	FacilityType* type;
	NetworkLink* father;
	LinkList children;
};


//---------------------------------------- 
//	NetworkLink Declaration
//----------------------------------------

class NetworkLink {
public:

	NetworkLink(unsigned int id, FacilityNode* father, FacilityNode* child,
			PSLProblem& problem, bool hierarchic);

	//Destructor of NetworkLink
	//Do not delete origin and destination
	//because these nodes are deleted in the destructor of PSLProblem
	//
	~NetworkLink() {}

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
	//void forEachPath(void(*ptr)(FacilityNode* n1, FacilityNode* n2)) const;
	template <typename FuncType>
	void forEachPath(FuncType functor) const;

	ostream& toDotty(ostream& out);

private:
	unsigned int id;
	FacilityNode *origin;
	FacilityNode *destination;
	unsigned int bandwidth;
	bool reliable;
};

//----------------------------------------
//	LinkIterator Declaration
//----------------------------------------

class LinkIterator : public std::iterator<std::forward_iterator_tag, FacilityNode> {
public:
	LinkIterator(FacilityNode* p);

	//Destructor of LinkIterator
	//Do not delete pointers of iterator
	//	
	~LinkIterator() {}

	LinkIterator(const LinkIterator& other) : current(other.current), end(other.end), queue(other.queue)  {}	

	// The assignment and relational operators are straightforward
	LinkIterator& operator=(const LinkIterator& other) {
		if(*this != other) {		
			current = other.current;
			end = other.end;
			queue = other.queue;
		}
		return *this;
	}

	bool operator==(const LinkIterator& other) {
		return current == other.current;
	}

	bool operator!=(const LinkIterator& other) {
		//cout << *current << "!= " << (other.current) << endl;
		return current != other.current;
	}

	LinkIterator& operator++();

	LinkIterator& operator++(int) {
		++(*this);
		return *this;
	}

	NetworkLink* operator*() {
		return *current;
	}

	NetworkLink* operator->() {
		return *current;
	}

private:
	LinkListIterator current;
	LinkListIterator end;
	deque<FacilityNode*> queue;
};

//---------------------------------------- 
//	NodeIterator Declaration
//----------------------------------------

class NodeIterator : public std::iterator<std::forward_iterator_tag, FacilityNode> {
public:	
	NodeIterator(FacilityNode* p) : node(p), clink(NULL), elink(NULL) {}

	//Destructor of NodeIterator
	//Do not delete pointers of iterator
	//	
	~NodeIterator() {}

	NodeIterator(const NodeIterator& other) : node(other.node), clink(other.clink), elink(other.elink) {}

	// The assignment and relational operators are straightforward
	NodeIterator& operator=(const NodeIterator& other) {
		if(*this != other) {		
			node = other.node;
			clink = other.clink;
			elink = other.elink;
		}		
		return *this;
	}

	bool operator==(const NodeIterator& other) {
		return (node == other.node);
	}

	bool operator!=(const NodeIterator& other) {
		return (node != other.node);
	}

	NodeIterator& operator++();

	NodeIterator& operator++(int) {
		++(*this);
		return *this;
	}

	FacilityNode* operator*() {
		return node;
	}

	FacilityNode* operator->() {
		return node;
	}

private:
	FacilityNode* node;
	LinkIterator clink;
	LinkIterator elink;

};

//----------------------------------------
//	AncestorIterator Declaration
//----------------------------------------

class AncestorIterator : public std::iterator<std::forward_iterator_tag, FacilityNode> {
public:
	AncestorIterator(FacilityNode* p) : node( p) {
		++(*this);
	}

	//Destructor of AncestorIterator
	//Do not delete pointers of iterator
	//
	~AncestorIterator() {}

	AncestorIterator(const AncestorIterator& other) : node(other.node) {}

	// The assignment and relational operators are straightforward
	AncestorIterator& operator=(const AncestorIterator& other) {
		if(*this != other) {
			node = other.node;
		}
		return *this;
	}

	bool operator==(const AncestorIterator& other) {
		return (node == other.node);
	}

	bool operator!=(const AncestorIterator& other) {
		return (node != other.node);
	}

	AncestorIterator& operator++();

	AncestorIterator& operator++(int) {
		++(*this);
		return *this;
	}

	FacilityNode* operator*() {
		return node;
	}

	FacilityNode* operator->() {
		return node;
	}

private:
	FacilityNode* node;

};

//----------------------------------------
//	PathIterator Declaration
//----------------------------------------

class PathIterator : public std::iterator<std::forward_iterator_tag, pair<FacilityNode*, FacilityNode*> > {
public:
	PathIterator(FacilityNode* p) : cnode(p->nbegin()), enode(p->nend()), cdest(p->nbegin()), edest(p->nend()) {
		cdest++;
	}

	//Destructor of PathIterator
	//Do not delete pointers of iterator
	//
	~PathIterator() {}

	PathIterator(const PathIterator& other) : cnode(other.cnode), enode(other.enode), cdest(other.cdest), edest(other.edest) {}

	// The assignment and relational operators are straightforward
	PathIterator& operator=(const PathIterator& other) {
		if(*this != other) {
			cnode= other.cnode;
			enode= other.enode;
			cdest = other.cdest;
			edest = other.edest;
		}
		return *this;
	}

	bool operator==(const PathIterator& other) {
		return (cnode == other.cnode) && (cdest== other.cdest);
	}

	bool operator!=(const PathIterator& other) {
		return (cnode != other.cnode) || (cdest!= other.cdest);
	}

	PathIterator& operator++();

	PathIterator& operator++(int) {
		++(*this);
		return *this;
	}

	pair<FacilityNode*, FacilityNode*> operator*() {
		return pair<FacilityNode*, FacilityNode*>(*cnode, *cdest);
	}

	pair<FacilityNode*, FacilityNode*> operator->() {
		return *(*this);
	}

private:
	NodeIterator cnode;
	NodeIterator enode;
	NodeIterator cdest;
	NodeIterator edest;

};
//----------------------------------------
//	PSLProblem Declaration
//----------------------------------------

class PSLProblem {
public:
	PSLProblem() : _groupCount(0), root(NULL), _nodeCount(0) {}

	//Destructor of PSLProblem
	//Delete all nodes of the tree
	//Delete all servers
	//Delete all facilities
	//
	~PSLProblem() {
		deleteTree(root);		
		for_each(servers.begin(), servers.end(), FonctorDeletePtr());
		for_each(facilities.begin(), facilities.end(), FonctorDeletePtr());
	}

	inline unsigned int getBandwidth(unsigned int idx) const {
		return bandwidths[idx];
	}
	inline unsigned int bandwidthCount() const {
		return bandwidths.size();
	}
	inline ServerType* getServer(int idx) {
		return servers[idx];
	}
	inline unsigned int serverTypeCount() const {
		return servers.size();
	}

	inline unsigned int groupCount() const {
		return _groupCount;
	}

	inline unsigned int facilityTypeCount() const {
		return facilities.size();
	}

	inline unsigned int levelCount() const {
		return levelNodeCounts.size();
	}

	void setSeed(const unsigned int seed);
	FacilityNode* generateNetwork();

	//generate Breadth-First Numbered Tree
	FacilityNode* generateNetwork(bool hierarchic);

	bool checkNetwork();
	bool checkNetworkHierarchy();
	inline FacilityNode* getRoot() const {
		return root;
	}

	inline unsigned int nodeCount() const {
		return _nodeCount;
	}

	inline unsigned int linkCount() const {
		return _nodeCount - 1;
	}

	inline unsigned int pathCount() const {
		return lengthCumulPathCounts.back();
	}

	inline unsigned int stageCount() const {
		return _groupCount + 1;
	}

	inline unsigned int rankCount() const {
		return endBij();
	}

	inline IntList getLevelNodeCounts() {
		return levelNodeCounts;
	}
	//For NodeIterator
	inline NodeIterator nbegin() { return root->nbegin();}
	NodeIterator nend() { return root->nend();}

	//For LinkIterator
	LinkIterator lbegin() { return root->lbegin();}
	LinkIterator lend() { return root->lend();}

	ostream& toRanks(ostream& out);
	ostream& toDotty(ostream& out);

	friend ostream& operator<<(ostream& out, const PSLProblem& f);
	friend istream& operator>>(istream& in, PSLProblem& problem);

	//----------------------------------------
	//	Rank Mapper (associates each variable to an uniue index)
	//----------------------------------------
	int rankX(FacilityNode *node) const {
		return node->getID();
	}
	int rankX(FacilityNode *node, unsigned int stype) const {
		assert(stype >= 0 && stype < serverTypeCount());
		return endX() + node->getID() * serverTypeCount() + stype;
	}

	int rankY(FacilityNode *node, unsigned int stage) const {
		assert(stage >= 0 && stage < stageCount());
		return endXk() + node->getID() * stageCount() + stage;
	}

	int rankZ(FacilityNode *node, unsigned int stage) const {
		assert(stage >= 0 && stage < stageCount());
		return endYi() + node->getID() * stageCount() + stage;
	}

	int rankY(NetworkLink *link, unsigned int stage) const {
		assert(stage >= 0 && stage < stageCount());
		return endZi() + link->getID() * stageCount() + stage;
	}

	int rankZ(FacilityNode *source, FacilityNode *destination, unsigned int stage) const {
		return endYij() + rank(source, destination, stage);
	}

	int rankB(FacilityNode *source, FacilityNode *destination, unsigned int stage) const {
		return endZij() + rank(source, destination, stage);
	}

	int rankZ(pair<FacilityNode*, FacilityNode* > const &path, unsigned int stage) const {
		return rankZ(path.first, path.second, stage);
	}

	int rankB(pair<FacilityNode*, FacilityNode* > const &path, unsigned int stage) const {
		return rankB(path.first, path.second, stage);
	}
private:

	inline int endX() const {
		return _nodeCount;
	}

	inline int endXk() const {
		return endX() + _nodeCount * serverTypeCount();
	}

	inline int endYi() const {
		return endXk() + _nodeCount * stageCount();
	}

	inline int endZi() const {
		return endYi() + _nodeCount * stageCount();
	}

	inline int endYij() const {
		return endZi() +  linkCount() * stageCount();
	}

	inline int endZij() const {
		return endYij() + pathCount() * stageCount();
	}

	inline int endBij() const {
		return endZij() + pathCount() * stageCount();
	}

	inline int rank(FacilityNode* source, FacilityNode* destination) const {
		int length = destination->getType()->getLevel() - source->getType()->getLevel();
		//path are ranked by length and their index using the bread-first numbered tree.
		return lengthCumulPathCounts[length-1] + (destination->getID() - levelCumulNodeCounts[length]);
	}

	inline int rank(FacilityNode* source, FacilityNode* destination, unsigned int stage) const {
		assert(stage >= 0 && stage < stageCount());
		return rank(source, destination) * stageCount() + stage;
	}

	//Delete tree from root node
	void deleteTree(FacilityNode* node) {
		levelNodeCounts.clear();
		levelCumulNodeCounts.clear();
		lengthCumulPathCounts.clear();
		_nodeCount = 0;
		if(node != NULL) {
			for ( size_t i = 0; i < node->getChildrenCount(); ++i ) {
				deleteTree(node->toChild(i)->getDestination());
			}
			delete node;
			node = NULL;
		}
	}


	IntList bandwidths;
	ServerTypeList servers;
	FacilityTypeList facilities;
	unsigned int _groupCount;
	FacilityNode* root;
	unsigned int _nodeCount;
	IntList levelNodeCounts;
	//number of nodes of level lower or equal than l;
	IntList levelCumulNodeCounts;
	//number of path of length lower or equal than l
	IntList lengthCumulPathCounts;


};

//---------------------------------------- 
//	ostream and istream Declaration
//----------------------------------------

ostream& operator<<(ostream& out, const PSLProblem& f);
istream& operator>>(istream& in, PSLProblem& s);

ostream& operator<<(ostream& out, const ServerType& s);
istream& operator>>(istream& in, ServerType& s);

ostream& operator<<(ostream& out, const FacilityType& f);

ostream& operator<<(ostream& out, const FacilityNode& n);
ostream& operator<<(ostream& out, const NetworkLink& l);

//----------------------------------------
//	template definition (must not be placed into cpp)
//----------------------------------------
template <typename FuncType>
inline void NetworkLink::forEachPath(FuncType func) const {
	//void NetworkLink::forEachPath(
	//void(*ptr)(FacilityNode *n1, FacilityNode *n2)) const {
	FacilityNode* ancestor = getDestination();
	FacilityList successors;
	do {
		ancestor = ancestor->getFather();
		successors.push_back(getDestination());
		while (!successors.empty()) {
			FacilityNode* successor = successors.back();
			successors.pop_back();
			func(ancestor, successor);
			if (!successor->isLeaf()) {
				for (unsigned int i = 0; i < successor->getChildrenCount();
						++i) {
					successors.push_back(successor->getChild(i));
				}
			}
		}
	} while (!ancestor->isRoot());
}

inline bool isReliablePath(const FacilityNode* origin, FacilityNode* destination) {
	while(destination) {
		if(destination == origin) return true;
		else if(destination->toFather()->isReliable()) {
			destination = destination->getFather();
		} else destination = NULL;
	}
	return false;
}

#endif /* NETWORK_HPP_ */
