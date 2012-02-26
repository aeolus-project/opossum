/**
 *  Copyright (c) 2011, Arnaud Malapert and Mohamed Rezgui
 *  All rights reserved.
 *  Redistribution and use in source and binary forms, with or without
 *  modification, are permitted provided that the following conditions are met:
 *
 *      * Redistributions of source code must retain the above copyright
 *        notice, this list of conditions and the following disclaime
 *      * Redistributions in binary form must reproduce the above copyright
 *        notice, this list of conditions and the following disclaimer in the
 *        documentation and/or other materials provided with the distribution.
 *      * Neither the name of Arnaud Malapert nor the
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

//---------------------------------------- 
//	FacilityNode Implementation
//----------------------------------------

FacilityNode* FacilityNode::getFather() const {
	return father->getOrigin();
}

FacilityNode* FacilityNode::getChild(unsigned int i) const {
	return children[i]->getDestination();
}

//For NodeIterator
NodeIterator FacilityNode::nbegin() {
	return NodeIterator(this);
}
NodeIterator FacilityNode::nend() {
	return NodeIterator(NULL);
}

//For LinkIterator
LinkIterator FacilityNode::lbegin() {
	return LinkIterator(this);
}

LinkIterator FacilityNode::lend() {
	return LinkIterator(NULL);
}


ostream & FacilityNode::toDotty(ostream & out) {
	out << getID();
	out << "[shape=record, label=\"{{" << getID() << "}|{"
			<< getType()->getTotalDemand() << "|"
			<< getType()->getTotalCapacity() << "}}\"];" << endl;
	if (!isRoot()) {
		toFather()->toDotty(out);
	}
	for (size_t i = 0; i < children.size(); ++i) {
		children[i]->getDestination()->toDotty(out);
	}
	return out;
}

bool FacilityNode::isReliableFromRoot() {
	return isRoot() ? true : toFather()->isReliable() && getFather()->isReliableFromRoot();
}

void FacilityNode::print(ostream& out) {
	string shift = string(2 * type->getLevel(), ' ');
	string sep = string(15, '-');
	if (children.size() > 0) {
		out << shift << children.size() << " " << sep << endl;
		for (size_t i = 0; i < children.size(); ++i) {
			out << shift << *children[i] << endl;
		}
		for (size_t i = 0; i < children.size(); ++i) {
			children[i]->getDestination()->print(out);
		}
	}
}

//---------------------------------------- 
//	FacilityType Implementation
//----------------------------------------

bool FacilityType::genRandomReliability(){
	return randd() < reliabilityProbability;
}

unsigned int FacilityType::genRandomBandwidthIndex() {
	double cum = 0;
	const double p = randd();
	for (unsigned int i = 0; i < bandwidthProbabilities.size(); ++i) {
		cum += bandwidthProbabilities[i];
		if (p <= cum) {
			return i;
		}
	}
	assert(cum == 1);
	exit(1);
}

unsigned int FacilityType::genRandomFacilities() {
	//cout << "B(" << binornd->distribution().t() <<  ", " << binornd->distribution().p() << ")" <<endl;
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
	const double p = randd();
	for (unsigned int i = 0; i <= maxIndex; ++i) {
		cum += bandwidthProbabilities[i] / tot;
		if (p <= cum) {
			return i;
		}
	}assert(cum == 1);
	exit(1);
}

#ifdef NDEBUG //Mode Release
mt19937 FacilityType::default_random_generator(static_cast<unsigned int>(std::time(NULL)));
#else //Mode Debug
mt19937 FacilityType::default_random_generator(SEED);
#endif

uniform_01<mt19937&, double> FacilityType::randd(default_random_generator);
variate_generator<mt19937&, binomial_distribution<> > FacilityType::fake_binornd(default_random_generator, binomial_distribution<>(1,1));


//Set a new seed for random generators
void FacilityType::setStaticSeed(const unsigned int seed) {
	default_random_generator.seed(seed);
	randd.base().seed(seed);
}

void FacilityType::setSeed(const unsigned int seed) {
	binornd->engine().seed(seed);
}

void FacilityType::setBinomial(unsigned int n, double p) {
	delete binornd;
	binornd = new variate_generator<mt19937&, binomial_distribution<> >(
			default_random_generator, binomial_distribution<>(n, p));
}

istream & FacilityType::read(istream & in, const PSLProblem& problem) {
	int tmp;
	in >> level;
	unsigned int n = problem.groupCount();
	while (demands.size() < n) {
		in >> tmp;
		demands.push_back(tmp);
	}
	n = problem.serverTypeCount();
	while (serverCapacities.size() < n) {
		in >> tmp;
		serverCapacities.push_back(tmp);
	}
	int t;
	in >> t;
	double p;
	in >> p;
	setBinomial(t, p);
	n = problem.bandwidthCount();
	double d;
	while (bandwidthProbabilities.size() < n) {
		in >> d;
		bandwidthProbabilities.push_back(d);
	}
	in >> reliabilityProbability;
	return in;
}


//---------------------------------------- 
//	PSLProblem Implementation
//----------------------------------------

FacilityNode* PSLProblem::generateNetwork() {
	return generateNetwork(true);
}


FacilityNode* PSLProblem::generateNetwork(bool hierarchic) {
	//Delete old tree.
	deleteTree(root);
	levelNodeCounts.push_back(1);
	queue<FacilityNode*> queue;
	root = new FacilityNode(_nodeCount++, facilities[0]);
	queue.push(root);
	unsigned int ftype = 1, clevel = 0, idx = 0;
	FacilityNode* current = queue.front();
	do {
		queue.pop();
		idx = ftype;
		while (idx < facilities.size()
				&& facilities[idx]->getLevel() == clevel + 1) {
			//number of children
			const unsigned int nbc = facilities[idx]->genRandomFacilities();
			//generate children
			for (unsigned int i = 0; i < nbc; ++i) {
				FacilityNode* child = new FacilityNode(_nodeCount,
						facilities[idx]);
				new NetworkLink(_nodeCount - 1, current, child, *this,
						hierarchic);
				queue.push(child);
				_nodeCount++;
			}
			idx++;
		}
		if (queue.empty()) {
			break;
		}
		current = queue.front();
		if (current->getType()->getLevel() == clevel + 1) {
			levelNodeCounts.push_back(queue.size()); //Add number of facilities at level clevel
			clevel = current->getType()->getLevel();
			ftype = idx;
		} else
			assert(current->getType()->getLevel() == clevel);
	} while (ftype < facilities.size());

	//clear queue
	//class std::queue has no clear method
	while (!queue.empty()) {
		queue.pop();
	}
	//Initialize Rank Mapper Arrays
	levelCumulNodeCounts.push_back(0);
	lengthCumulPathCounts.push_back(0);
	for (unsigned int l = 0; l < levelNodeCounts.size(); ++l) {
		levelCumulNodeCounts.push_back( levelCumulNodeCounts.back() + levelNodeCounts[l]);
		lengthCumulPathCounts.push_back( lengthCumulPathCounts.back() + _nodeCount - levelCumulNodeCounts.back());
	}
	assert(checkNetwork() && ( !hierarchic || checkNetworkHierarchy() ));
	return root;
}


bool PSLProblem::checkNetwork()
{
	unsigned int sum = 0;
	//Test 1
	for (IntListIterator j = levelNodeCounts.begin();
			j != levelNodeCounts.end(); ++j)
		sum += *j;
	if( sum != _nodeCount) return false;

	sum = 0;
	for (NodeIterator n = root->nbegin(); n != root->nend(); ++n) {
		sum ++;
	}
	if( sum != _nodeCount) return false;

	//Test 3
	sum = 0;
	for (LinkIterator n = root->lbegin(); n != root->lend(); ++n) {
		sum ++;
	}
	if( sum != linkCount()) return false;

	return true;
}

bool PSLProblem::checkNetworkHierarchy()
{
	for (LinkIterator n = root->lbegin(); n != root->lend(); ++n) {
		if(! n->getOrigin()->isRoot() && (
				( n->isReliable() && ! n->getOrigin()->toFather()->isReliable()) ||
				n->getBandwidth() > n->getOrigin()->toFather()->getBandwidth()
		)
		) {
			return false;
		}
	}
	return true;
}

void PSLProblem::setSeed(const unsigned int seed)
{
	FacilityType::setStaticSeed(seed);
	for(FacilityTypeListIterator i=facilities.begin() ; i!=facilities.end() ; i++) {
		(*i)->setSeed(seed);
	}
}

ostream& PSLProblem::toDotty(ostream & out) {
	if (root) {
		out << "digraph G {\n";
		root->toDotty(out);
		out << "}\n";
	}
	return out;
}


ostream& PSLProblem::toRanks(ostream & out) {
	out << "X=[0," << offsetXk() << "[" << endl;
	out << "Xk=[" << offsetXk() << "," << offsetYi() << "[" << endl;
	out << "Yi=[" << offsetYi() << "," << offsetYij() << "[" << endl;
	out << "Yij=[" << offsetYij() << "," << offsetZ() << "[" << endl;
	out << "Z=[" << offsetZ() << "," << offsetB() << "[" << endl;
	out << "B=[" << offsetB() << "," << rankCount() << "[" << endl;
	return out;
}



//---------------------------------------- 
//	NetworkLink Implementation
//----------------------------------------

NetworkLink::NetworkLink(unsigned int id, FacilityNode* father,
		FacilityNode* child, PSLProblem& problem, bool hierarchic) :
		id(id), origin(father), destination(child), bandwidth(0), reliable(0) {
	father->children.push_back(this);
	child->father = this;
	if (father->isRoot() || !hierarchic) {
		bandwidth = problem.getBandwidth(
				child->getType()->genRandomBandwidthIndex());
		reliable = child->getType()->genRandomReliability();
	} else {
		unsigned int fbandw = father->toFather()->getBandwidth();
		int maxIndex = problem.bandwidthCount() - 1;
		while (maxIndex >= 0 && problem.getBandwidth(maxIndex) > fbandw) {
			maxIndex--;
		}
		bandwidth = problem.getBandwidth(
				child->getType()->genRandomBandwidthIndex(maxIndex));
		reliable = father->toFather()->isReliable() && child->getType()->genRandomReliability();
	}
}

ostream& NetworkLink::toDotty(ostream & out) {
	out << origin->getID() << " -> " << destination->getID();
	out << "[label=\"" << getBandwidth() << "\"";
	if (isReliable()) {
		out << ", style=bold ";
	}
	out << "];" << endl;
	return out;
}



//----------------------------------------
//	LinkIterator Implementation
//----------------------------------------

LinkIterator::LinkIterator(FacilityNode* p) : current(NULL), end(NULL) {
	if (p && !p->isLeaf()) {
		current = p->cbegin();
		end = p->cend();
	}
}

LinkIterator& LinkIterator::operator ++() {
	queue.push_back((*current)->getDestination());
	current++;
	while (current == end && !queue.empty()) {
		current = queue.front()->cbegin();
		end = queue.front()->cend();
		queue.pop_front();
	}
	return (*this);
}

//----------------------------------------
//	NodeIterator Implementation
//----------------------------------------

NodeIterator& NodeIterator::operator++() {
	if (node != NULL) {
		if (clink == NULL) {
			clink = node->lbegin();
			elink = node->lend();
		}
		if (clink != elink) {
			node = clink->getDestination();
			clink++;
		} else {
			node = NULL;
		}
	}
	return (*this);
}


//----------------------------------------
//	istream methods Implementation
//----------------------------------------

istream& operator >>(istream & in, PSLProblem & problem) {
	int n;
	in >> n;
	for (int i = 0; i < n; ++i) {
		int bandwidth;
		in >> bandwidth;
		problem.bandwidths.push_back(bandwidth);
	}
	in >> n;
	for (int i = 0; i < n; ++i) {
		ServerType* stype = new ServerType();
		in >> *stype;
		problem.servers.push_back(stype);
	}
	in >> problem._groupCount;
	in >> n;
	for (int i = 0; i < n; ++i) {
		FacilityType* ftype = new FacilityType();
		ftype->read(in, problem);
		problem.facilities.push_back(ftype);
	}
	return in;
}

istream& operator >>(istream & in, ServerType & s) {
	in >> s.capacity >> s.cost;
	return in;
}

//----------------------------------------
//	ostream methods Implementation
//----------------------------------------

ostream& operator <<(ostream & out, const PSLProblem & f) {
	cout << "Bandwidths: {";
	copy(f.bandwidths.begin(), f.bandwidths.end(),
			ostream_iterator<int>(out, " "));
	cout << "}" << endl;
	transform(f.servers.begin(), f.servers.end(),
			ostream_iterator<ServerType>(out, "\n"), dereference<ServerType>);
	transform(f.facilities.begin(), f.facilities.end(),
			ostream_iterator<FacilityType>(out, "\n"),
			dereference<FacilityType>);
	if (f.root)
		f.root->print(out);
	return out;
}

ostream& operator <<(ostream& out, const NetworkLink& l) {
	return out << *l.getOrigin() << " -> " << *l.getDestination() << " ("
			<< l.getBandwidth() << ", " << l.isReliable() << ")";

}

ostream& operator <<(ostream& out, const FacilityNode& n) {
	return out << n.getID(); // << " " << n.getChildrenCount();
}


ostream& operator<<(ostream& out, const FacilityType& f) {

	out << "Level:" << f.getLevel() << "\tDemand:{ ";
	copy(f.demands.begin(), f.demands.end(), ostream_iterator<int>(out, " "));
	out << "}\tCapacities:{ ";
	copy(f.serverCapacities.begin(), f.serverCapacities.end(),
			ostream_iterator<int>(out, " "));
	out << "}" << endl << "Probabilities -> Facilities:B("
			<< f.binornd->distribution().t() << ","
			<< f.binornd->distribution().p() << ")\tBandwidths:{ ";
	copy(f.bandwidthProbabilities.begin(), f.bandwidthProbabilities.end(),
			ostream_iterator<double>(out, " "));
	out << "}\tReliability:" << f.reliabilityProbability;
	return out;

}

ostream& operator<<(ostream& out, const ServerType& f) {
	return out << "Capacity:" << f.getMaxConnections() << "\tCost:" << f.getCost();
}


