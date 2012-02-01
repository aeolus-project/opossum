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
#include <assert.h>

using namespace std;

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
	FacilityType() : level(0), demand(0), binoN(1), binoP(1), reliabilityProbability(1)
	{}
	virtual ~FacilityType() {
		//FIXME delete [] serverCapacities; delete [] bandwidthProbabilities;
	}
	inline unsigned int getLevel() const { return level; }
	inline unsigned int getDemand() const { return demand; }
	inline unsigned int getServerCapacity(const unsigned int stype) const {return serverCapacities[stype];
	}
	unsigned int getConnexionCapacity(const vector<ServerType*>* servers) const;
	//string toGEXF();
	unsigned int genRandomFacilities();
	unsigned int genRandomBandwidthIndex();
	unsigned int genRandomBandwidthIndex(unsigned int maxBandwidth);
	bool genRandomReliability();

	istream& read(istream& in, const PSLProblem& problem);
	friend ostream& operator<<(ostream& out, const FacilityType& f);
protected:
	//inline double getBandwitdhProbability(const unsigned int stype) const { return bandwidthProbabilities[stype];}
	//inline double getReliabilityProbability() const { return reliabilityProbability; }

private:
	unsigned int level;
	unsigned int demand;
	vector<unsigned int> serverCapacities;
	unsigned int binoN;
	double binoP;
	vector<double> bandwidthProbabilities;
	double reliabilityProbability;

};

class PSLProblem {

public:
	PSLProblem() {
		//TODO ctor
	}
	~PSLProblem() {
		//TODO dtor
	}
	vector<unsigned int> bandwidths;
	vector<ServerType*> servers;
	vector<FacilityType*> facilities;

	//FIXME probleme de copie
	//	inline vector< unsigned int> getBandwidths() {
	//		return bandwidths;
	//	}
	//	inline vector<FacilityType*> getFacilities() const {
	//		return facilities;
	//	}
	//	inline vector<ServerType*> getServers() const {
	//		return servers;
	//	}

	//TODO déclarer deux fois les méthodes amies ?
	friend istream& operator>>(istream& in, PSLProblem& problem);
protected:
private:



};

class NetworkLink;


class FacilityNode
{
	friend class NetworkLink;  // bof !!
	static unsigned int NEXT_ID;

public:
	FacilityNode(FacilityType* type) : id(NEXT_ID++), type(type), father(NULL) {
	}
	~FacilityNode() {
		//TODO dtor;
	}
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
	ostream& toGEXF(ostream& out);
	void printSubtree();
protected:
private:
	unsigned int id;
	FacilityType* type;
	NetworkLink* father;
	vector<NetworkLink*> children;


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
			bandwidth = problem.bandwidths[child->getType()->genRandomBandwidthIndex()];
			reliable = child->getType()->genRandomReliability();
		} else {
			unsigned int fbandw = father->toFather()->getBandwidth();
			int maxIndex = problem.bandwidths.size()-1;
			while( maxIndex >= 0 && problem.bandwidths[maxIndex] > fbandw) {maxIndex--;}
			bandwidth = problem.bandwidths[child->getType()->genRandomBandwidthIndex(maxIndex)];
			reliable = father->toFather()->isReliable() && child->getType()->genRandomReliability();
		}
	}

~NetworkLink() {
}
inline FacilityNode* getOrigin() const { return origin; }
inline FacilityNode* getDestination() const { return destination; }
inline bool isReliable() const { return reliable; }
inline unsigned int getBandwidth() const { return bandwidth; }
void forEachPath() const;
void forEachPath(void (*ptr)(FacilityNode* n1, FacilityNode* n2)) const;
ostream& toDotty(ostream& out);
ostream& toGEXF(ostream& out);
//string* toGEXF();
protected:
private:
FacilityNode *origin;
FacilityNode *destination;
unsigned int bandwidth;
bool reliable;
};


//FacilityNode* generateSubtree(vector<FacilityType*> ftypes, unsigned int idx);
FacilityNode *generateSubtree(FacilityNode* root, PSLProblem& problem);

inline double randd();
//unsigned int randi(int min, int max);
unsigned int binornd(const int n, const double p);

template <typename T>
T& dereference(T* ptr) { return *ptr; }

ostream& operator<<(ostream& out, const PSLProblem& f);
istream& operator>>(istream& in, PSLProblem& s);

ostream& operator<<(ostream& out, const ServerType& s);
istream& operator>>(istream& in, ServerType& s);

ostream& operator<<(ostream& out, const FacilityType& f);
//istream& operator>>(istream& in, FacilityType& f);


ostream& operator<<(ostream& out, const FacilityNode& n);
ostream& operator<<(ostream& out, const NetworkLink& l);


#endif /* NETWORK_HPP_ */
