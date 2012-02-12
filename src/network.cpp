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
#include "network.hpp"

#include "Utils.hpp"

FacilityNode::~FacilityNode() {
	delete type;
	delete father;
	for_each(children.begin(), children.end(), Delete());
}


unsigned int FacilityNode::NEXT_ID=0;


unsigned int FacilityNode::getMinIncomingConnections(vector<ServerType*>* servers) {
	const unsigned int capa = type->getConnexionCapacity(servers);
	unsigned int res = type->getDemand() >  capa ? type->getDemand() -  capa : 0 ;
	for ( size_t i = 0; i < children.size(); ++i ) {
		res+= children[i]->getDestination()->getMinIncomingConnections(servers);
	}
	return res;
}


ostream & FacilityNode::toDotty(ostream & out)
{
	out << getID();
	//FIXME : Wait for the implementation of new methods : getTotalDemand and getTotalCapacity
	//out << "[shape=record, label=\"{{" <<  getID() << "}|{" << getType()->getTotalDemand() << "|" << getType()->getTotalCapacity() << "}}\"];" << endl;
	out << "[shape=record, label=\"{{" <<  getID() << "}}\"];" << endl;
	if(! isRoot()) {
		   toFather()->toDotty(out);
	}
	for ( size_t i = 0; i < children.size(); ++i ) {
		   children[i]->getDestination()->toDotty(out);
	}
	return out;
}



FacilityNode *FacilityNode::getFather() const
{
	return father->getOrigin();
}

inline FacilityNode *FacilityNode::getChild(unsigned int i) const
{
	return children[i]->getDestination();
}

void FacilityNode::printSubtree() {
	string shift = string(2 * type->getLevel(),' ');
	string sep = string(15,'-');
	if(children.size() > 0) {
		cout << shift << children.size() << " " << sep << endl;
		for ( size_t i = 0; i < children.size(); ++i ) {
			cout << shift <<  *children[i] << endl;
		}
		for ( size_t i = 0; i < children.size(); ++i ) {
			children[i]->getDestination()->printSubtree();
		}
	}
}

bool FacilityType::genRandomReliability()
{
	return (*randd)() < reliabilityProbability;
}


unsigned int FacilityType::genRandomBandwidthIndex()
{
	double cum = 0;
	const double p = (*randd)();
	for (unsigned int i = 0; i < bandwidthProbabilities.size(); ++i) {
		cum+= bandwidthProbabilities[i];
		if(p <= cum) {return i;}
	}
	assert(cum == 1);
	exit (1);
}

unsigned int FacilityType::genRandomFacilities()
{
	cout << "B(" << binornd->distribution().t() <<  ", " << binornd->distribution().p() << ")" <<endl;
	return (*binornd)();
}


unsigned int FacilityType::genRandomBandwidthIndex(unsigned int maxIndex) {
	double tot = 0;
	//compute total probability
	for (unsigned int i = 0; i <= maxIndex; ++i) {
		tot += bandwidthProbabilities[i];
	}
	//compute random values using normalized probabilities
	double cum = 0;
	const double p = (*randd)();
	for (unsigned int i = 0; i <= maxIndex; ++i) {
		cum+= bandwidthProbabilities[i]/tot;
		if(p <= cum) { return i;}
	}
	assert(cum == 1);
	exit (1);
}

#ifdef NDEBUG //Mode Release
mt19937 FacilityType::default_random_generator(static_cast<unsigned int>(std::time(NULL)));
#else //Mode Debug
mt19937 FacilityType::default_random_generator(SEED);
#endif
variate_generator<mt19937&, binomial_distribution<> > FacilityType::fake_binornd(default_random_generator, binomial_distribution<>(1,1));

//Set a new seed for random generator
void FacilityType::setSeed(int seed) {
	random_generator.seed(seed);
	delete randd;
	randd = new uniform_01< mt19937&, double >(random_generator);
	int t = binornd->distribution().t();
	int p = binornd->distribution().p();
	delete binornd;
	binornd = new variate_generator<mt19937&, binomial_distribution<> >(random_generator, binomial_distribution<>(t,p));
}

istream & FacilityType::read(istream & in, const PSLProblem& problem)
{
	in >> level >> demand;
	unsigned int n = problem.getNbServers();
	int tmp;
	while(serverCapacities.size() < n) {
		in >> tmp;
		serverCapacities.push_back(tmp);
	}
	int t;
	in >> t;
	double p;
	in >> p;
	delete binornd;
	binornd = new variate_generator<mt19937&, binomial_distribution<> >(random_generator, binomial_distribution<>(t,p));
	n = problem.getNbBandwidths();
	double d;
	while(bandwidthProbabilities.size() < n) {
		in >> d;
		bandwidthProbabilities.push_back(d);
	}
	in >> reliabilityProbability;
	return in;
}

unsigned int FacilityType::getConnexionCapacity(const vector<ServerType*>* servers) const
{
	unsigned int tot = 0;
	for (unsigned int i = 0; i < servers->size(); ++i) {
		tot+= serverCapacities[i] *  (*servers)[i]->getMaxConnections();
	}
	return tot;
}

ostream& operator<<(ostream& out, const FacilityType& f) {

	out << "Level:" << f.getLevel() << "\tDemand:" << f.getDemand() << "\tCapacities:{ ";
	copy(f.serverCapacities.begin(), f.serverCapacities.end(), ostream_iterator<int>(out, " "));
	out << "}" << endl << "Probabilities -> Facilities:B(" << f.binornd->distribution().t() << "," << f.binornd->distribution().p() <<")\tBandwidths:{ ";
	copy(f.bandwidthProbabilities.begin(), f.bandwidthProbabilities.end(), ostream_iterator<double>(out, " "));
	out << "}\tReliability:" << f.reliabilityProbability;
	return out;

}

ostream& operator<<(ostream& out, const ServerType& f) {
	return out << "Capacity:" << f.getMaxConnections() << "\tCost:" << f.getCost();
}


istream & operator >>(istream & in, ServerType & s)
{
	in >> s.capacity >> s.cost;
	return in;
}


ostream & operator <<(ostream & out, const PSLProblem & f)
{
	cout << "Bandwidths: {";
	copy(f.bandwidths.begin(), f.bandwidths.end(), ostream_iterator<int>(out, " "));
	cout << "}" << endl ;
	transform(f.servers.begin(), f.servers.end(), ostream_iterator<ServerType>(out, "\n"), dereference<ServerType>);
	transform(f.facilities.begin(), f.facilities.end(), ostream_iterator<FacilityType>(out, "\n"), dereference<FacilityType>);
	return out;
}

FacilityNode *PSLProblem::generateNetwork()
{
	FacilityNode* root = new FacilityNode(facilities[0]);
	return generateSubtree(root);
}

FacilityNode *PSLProblem::generateSubtree(FacilityNode *root)
{
	unsigned int idx = 0;
	const unsigned int level = root->getType()->getLevel();
	const unsigned int n = facilities.size() - 1;
	while(idx  < n && facilities[idx]->getLevel() <= level) {idx++;}
	while(idx  < n && facilities[idx]->getLevel() == level + 1) {
		const unsigned int nbc = facilities[idx]->genRandomFacilities();
		for (unsigned int i = 0; i < nbc; ++i) {
			FacilityNode* child = new FacilityNode(facilities[idx]);
			new NetworkLink(root, child, *this, false);
			generateSubtree(child);
		}
		idx++;
	}
	return root;
}

istream & operator >>(istream & in, PSLProblem & problem)
{
	int n;
	in >> n;
	for (int i = 0; i < n; ++i) {
		int bandwidth;
		in >> bandwidth;
		problem.bandwidths.push_back( bandwidth);
	}
	in >> n;
	for (int i = 0; i < n; ++i) {
		ServerType* stype = new ServerType();
		in >> *stype;
		problem.servers.push_back(stype);
	}
	in >> n;
	for (int i = 0; i < n; ++i) {
		FacilityType* ftype = new FacilityType();
		ftype->read(in, problem);
		problem.facilities.push_back(ftype);
	}
	return in;
}

ostream & operator <<(ostream& out, const NetworkLink& l)
{
	return out << *l.getOrigin() << " -> " << *l.getDestination() <<" (" << l.getBandwidth() << ", " << l.isReliable() <<")";

}

ostream & operator <<(ostream& out, const FacilityNode& n)
{
	return out << n.getID(); // << " " << n.getChildrenCount();

}




void NetworkLink::forEachPath() const
{
	FacilityNode* ancestor = getDestination();
	vector<FacilityNode*> successors;
	do {
		ancestor= ancestor->getFather();
		successors.push_back(getDestination());
		while( ! successors.empty() ) {
			FacilityNode* successor= successors.back();
			successors.pop_back();
			cout << *ancestor << " -> " << *successor<< endl;
			if(! successor->isLeaf()) {
				for (unsigned int i = 0; i < successor->getChildrenCount(); ++i) {
					successors.push_back(successor->getChild(i));
				}
			}
		}
	} while(! ancestor->isRoot());

}

ostream & NetworkLink::toDotty(ostream & out)
{
	out << origin->getID() << " -> " << destination->getID();
	out << "[label=\"" << getBandwidth() << "\"";
	if(isReliable()) {
		out << ", style=bold ";
	}
	out << "];" <<endl;
	return out;
}


void NetworkLink::forEachPath(void(*ptr)(FacilityNode *n1, FacilityNode *n2)) const
{
	FacilityNode* ancestor = getDestination();
	vector<FacilityNode*> successors;
	do {
		ancestor= ancestor->getFather();
		successors.push_back(getDestination());
		while( ! successors.empty() ) {
			FacilityNode* successor= successors.back();
			successors.pop_back();
			(*ptr)(ancestor, successor);
			if(! successor->isLeaf()) {
				for (unsigned int i = 0; i < successor->getChildrenCount(); ++i) {
					successors.push_back(successor->getChild(i));
				}
			}
		}
	} while(! ancestor->isRoot());
}















