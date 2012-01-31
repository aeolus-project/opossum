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
#include <vector>

using namespace std;

class ServerType
{
public:
	ServerType(unsigned int capacity, unsigned int cost) {
		this->capacity = capacity;
		this->cost = cost;
	}

	virtual ~ServerType() {};
	inline unsigned int getCost() const { return cost; }
	inline unsigned int getMaxConnections() const { return capacity; }
protected:
private:
	unsigned int cost;
	unsigned int capacity;
};


static const unsigned int nbServerTypes = 2;
static const ServerType* serverTypes[] = { new ServerType(500,5), new ServerType(100,2) };

static const unsigned int nbBandwidths=4;
static const unsigned int bandwidths[] = {1000,100,20,2};

//static const unsigned int nbColors =9;
//static const string colors[] ={"firebrick", "forestgreen", "goldenrod", "navyblue",
//		"darkviolet", "brown", "darkorange","cyan", "lightblue"};



class FacilityType
{
public:
	FacilityType() : level(0), demand(0), serverCapacities(NULL), binoN(1), binoP(1), bandwidthProbabilities(NULL), reliabilityProbability(1)
	{}
	FacilityType(unsigned int level, unsigned int demand, unsigned int serverCapacities[], unsigned int binoN, double binoP, double bandwidthProbabilities[],double reliabilityProbability) : level(level), demand(demand), serverCapacities(serverCapacities), binoN(binoN), binoP(binoP), bandwidthProbabilities(bandwidthProbabilities), reliabilityProbability(reliabilityProbability)
	{}
	virtual ~FacilityType() { delete [] serverCapacities; delete [] bandwidthProbabilities; }
	inline unsigned int getLevel() const { return level; }
	inline unsigned int getDemand() const { return demand; }
	inline unsigned int getServerCapacity(const unsigned int stype) const {
		return stype < nbServerTypes ? serverCapacities[stype] : 0;
	}
	inline unsigned int getConnexionCapacity() const {
		unsigned int tot = 0;
		for (unsigned int i = 0; i < nbServerTypes; ++i) {
			tot+= serverCapacities[i] * serverTypes[i]->getMaxConnections();
		}
		return tot;
	}
	//string toGEXF();
	unsigned int genRandomFacilities();
	unsigned int genRandomBandwidth();
	unsigned int genRandomBandwidth(unsigned int maxBandwidth);
	bool genRandomReliability();

	friend ostream& operator<<(ostream& out, const FacilityType& f);
protected:
	inline double getBandwitdhProbability(const unsigned int stype) const {
		return stype < nbBandwidths? bandwidthProbabilities[stype] : 0;
	}
	inline double getReliabilityProbability() const { return reliabilityProbability; }

private:
	unsigned int level;
	unsigned int demand;
	unsigned int* serverCapacities;
	unsigned int binoN;
	double binoP;
	double* bandwidthProbabilities;
	double reliabilityProbability;

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
		// delete children; ??
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
	unsigned int getMinIncomingConnections();
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
	NetworkLink(FacilityNode* father, FacilityNode* child) : origin(father), destination(child), bandwidth(0),reliable(0)
	{
		father->children.push_back(this);
		child->father=this;

		//Is the link from the grandfather to the father reliable ?
		if( father->isRoot() ) {
			bandwidth = child->getType()->genRandomBandwidth();
			reliable = child->getType()->genRandomReliability();
		} else {
			bandwidth = child->getType()->genRandomBandwidth(father->toFather()->getBandwidth());
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
FacilityNode *generateSubtree(FacilityNode* root, const vector<FacilityType*> ftypes);

inline double randd();
//unsigned int randi(int min, int max);
unsigned int binornd(const int n, const double p);


ostream& operator<<(ostream& out, const ServerType& s);
ostream& operator<<(ostream& out, const FacilityType& f);
ostream& operator<<(ostream& out, const FacilityNode& n);
ostream& operator<<(ostream& out, const NetworkLink& l);


#endif /* NETWORK_HPP_ */
