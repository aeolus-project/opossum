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


double randd()
{
	return rand() / (double) RAND_MAX;
}

unsigned int binornd(const int n, const double p)
{
	unsigned int r=0;
	for (int i = 0; i < n; ++i) {
		if(randd() < p) { r++; }
	}
	return r;
}


unsigned int FacilityType::genRandomFacilities()
{
	return binornd(binoN, binoP);
}


unsigned int FacilityType::genRandomBandwidth()
{
	double cum = 0;
	const double p = randd();
	for (unsigned int i = 0; i < problem->bandwidths.size(); ++i) {
		cum+=getBandwitdhProbability(i);
		if(p <= cum) {return problem->bandwidths[i];}
	}
	return -1;
}

unsigned int FacilityType::genRandomBandwidth(unsigned int maxBandwidth) {
	unsigned int idx = 0;
	unsigned int n = problem->bandwidths.size();
	//find index
	while(idx < n &&
			problem->bandwidths[idx] > maxBandwidth) {
		idx++;
	}
	double tot = 0;
	//compute total probability
	for (unsigned int i = idx; i < n; ++i) {
		tot += getBandwitdhProbability(i);
	}
	//compute random values using normalized probabilities
	double cum = 0;
	const double p = randd();
	for (unsigned int i = idx; i < n; ++i) {
		cum+= getBandwitdhProbability(i)/tot;
		if(p <= cum) {return problem->bandwidths[i];}
	}
	return -1;
}

bool FacilityType::genRandomReliability()
{
	return randd() < reliabilityProbability;
}

unsigned int FacilityNode::NEXT_ID=0;


//FacilityNode *generateSubtree(const vector<FacilityType*> ftypes, unsigned int idx)
//{
//	FacilityNode* root = new FacilityNode(ftypes[idx]);
//	const unsigned int level = ftypes[idx]->getLevel();
//	while(idx  < ftypes.size() && ftypes[idx]->getLevel() == level) {idx++;}
//	while(idx  < ftypes.size() && ftypes[idx]->getLevel() == level + 1) {
//		const unsigned int nbc = ftypes[idx]->genRandomFacilities();
//		for (unsigned int i = 0; i < nbc; ++i) {
//			FacilityNode* child = generateSubtree(ftypes, idx);
//			new NetworkLink(root, child);
//		}
//		idx++;
//	}
//	return root;
//}

FacilityNode *generateSubtree(FacilityNode* root, const vector<FacilityType*> ftypes)
{
	unsigned int idx = 0;
	const unsigned int level = root->getType()->getLevel();
	while(idx  < ftypes.size() && ftypes[idx]->getLevel() <= level) {idx++;}
	while(idx  < ftypes.size() && ftypes[idx]->getLevel() == level + 1) {
		const unsigned int nbc = ftypes[idx]->genRandomFacilities();
		for (unsigned int i = 0; i < nbc; ++i) {
			FacilityNode* child = new FacilityNode(ftypes[idx]);
			new NetworkLink(root, child);
			generateSubtree(child, ftypes);
		}
		idx++;
	}
	return root;
}

unsigned int FacilityNode::getMinIncomingConnections() {
	unsigned int res = type->getDemand() >  type->getConnexionCapacity() ? type->getDemand() -  type->getConnexionCapacity() : 0 ;
	//cout << res<<endl;
	for ( size_t i = 0; i < children.size(); ++i ) {
		res+= children[i]->getDestination()->getMinIncomingConnections();
	}
	return res;
}


ostream & FacilityNode::toDotty(ostream & out)
{
	out << getID();
	out << "[label=\"" <<  getType()->getDemand() << "\"];" << endl;
	if(! isRoot()) {
		toFather()->toDotty(out);
	}
	for ( size_t i = 0; i < children.size(); ++i ) {
		children[i]->getDestination()->toDotty(out);
	}
	return out;
}

ostream & FacilityNode::toGEXF(ostream & out)
{
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


istream & FacilityType::read(istream & in, const PSLProblem& problem)
{
	in >> level >> demand;
	int n = problem.servers.size();
	serverCapacities = new unsigned int[n];
	for (int i = 0; i < n; ++i) {
		in >> serverCapacities[i];
	}
	in >> binoN >> binoP;
	n = problem.bandwidths.size();
	bandwidthProbabilities = new double[n];
	for (int i = 0; i < n; ++i) {
		in >> bandwidthProbabilities[i];
	}
	in >> reliabilityProbability;
	return in;
}

unsigned int FacilityType::getConnexionCapacity() const
{
		unsigned int tot = 0;
		for (unsigned int i = 0; i < problem->servers.size(); ++i) {
			tot+= serverCapacities[i] * problem->servers[i]->getMaxConnections();
		}
		return tot;
	}

ostream& operator<<(ostream& out, const FacilityType& f) {

	out << "Level:" << f.getLevel() << "\tDemand:" << f.getDemand() << "\tCapacities:";
	for (unsigned int i = 0; i < problem->servers.size(); ++i) {
		//FIXME retrieve array size issue
		cout << f.getServerCapacity(i) << " ";
	}
	out << endl << "Probabilities -> Facilities:B(" << f.binoN << "," << f.binoP <<")\tBandwidths:";
	for (unsigned int i = 0; i < problem->bandwidths.size(); ++i) {
		//FIXME retrieve array size issue
		cout << f.getBandwitdhProbability(i) << " ";
	}
	out << "\tReliability:" << f.getReliabilityProbability();
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

istream & operator >>(istream & in, PSLProblem & problem)
{
	int n;
		in >> n;
		for (int i = 0; i < n; ++i) {
			int bandwidth;
			in >> bandwidth;
			problem.bandwidths.push_back( bandwidth);
			//cout << bandwidth <<endl;
		}
		in >> n;
		for (int i = 0; i < n; ++i) {
			ServerType* stype = new ServerType();
			in >> *stype;
			problem.servers.push_back(stype);
			//cout << *stype <<endl;
		}
		in >> n;
		for (int i = 0; i < n; ++i) {
			FacilityType* ftype = new FacilityType();
			ftype->read(in, problem);
			problem.facilities.push_back(ftype);
			//cout << *ftype <<endl;
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

ostream & NetworkLink::toGEXF(ostream & out)
{
	return out;
}










