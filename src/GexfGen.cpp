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

#include "GexfGen.hpp"
#include "Utils.hpp"

AbstractGexfGen::AbstractGexfGen(FacilityNode* root) : _root(root) {
}

void AbstractGexfGen::initGexf(libgexf::GEXF* gexf) {

	initFaciltyNode(_root, gexf);

    // meta data
    //libgexf::MetaData& meta = gexf->getMetaData();
	//meta.setCreator("Opossum Instance");
	//meta.setDescription("This is an network instance by opossum");
}

void AbstractGexfGen::writeToFile(string pathFile) {

	if(_root == NULL) {
		cout << "Problem : the Facility root is NULL" << endl;
		return;
	}

	libgexf::GEXF* gexf = new libgexf::GEXF();

	this->initGexf(gexf);

	// write gexf file
    libgexf::FileWriter *writer = new libgexf::FileWriter();
    writer->init(pathFile, gexf);
    writer->write();
}

InstanceGexfGen::InstanceGexfGen(FacilityNode* root) : AbstractGexfGen(root) {}

void InstanceGexfGen::initFaciltyNode(FacilityNode* node, libgexf::GEXF* gexf) {
	if(gexf == NULL) {
		cout << "Problem NULL Pointer with gexf" << endl;
		return;
	}

	libgexf::DirectedGraph& graph = gexf->getDirectedGraph();
//	libgexf::UndirectedGraph& graph = gexf->getUndirectedGraph();

	graph.addNode(Utils::convert2str(node->getID()));

	libgexf::Data& data = gexf->getData();
	data.setNodeLabel(Utils::convert2str(node->getID()), Utils::convert2str(node->getType()->getDemand()));

	if(!node->isRoot()) {
		initNetworkLink(node->toFather(), gexf);
	}

	for ( size_t i = 0; i < node->getChildrenCount(); ++i ) {
		initFaciltyNode(node->toChild(i)->getDestination(), gexf);
	}
}

void InstanceGexfGen::initNetworkLink(NetworkLink* network, libgexf::GEXF* gexf) {
	if(gexf == NULL) {
		cout << "Problem NULL Pointer with gexf" << endl;
		return;
	}

	libgexf::DirectedGraph& graph = gexf->getDirectedGraph();
//	libgexf::UndirectedGraph& graph = gexf->getUndirectedGraph();

	string idEdge(Utils::convert2str(network->getOrigin()->getID()) + "-" + Utils::convert2str(network->getDestination()->getID()));
	graph.addEdge(idEdge,
					Utils::convert2str(network->getOrigin()->getID()),
					Utils::convert2str(network->getDestination()->getID()));

	libgexf::Data& data = gexf->getData();
	data.setEdgeLabel(idEdge, Utils::convert2str(network->getBandwidth()));

}

FlowConnectionsGexfGen::FlowConnectionsGexfGen(FacilityNode* root) : AbstractGexfGen(root) {
}

void FlowConnectionsGexfGen::initFaciltyNode(FacilityNode* node, libgexf::GEXF* gexf) {
	if(gexf == NULL) {
		cout << "Problem NULL Pointer with gexf" << endl;
		return;
	}

	libgexf::DirectedGraph& graph = gexf->getDirectedGraph();
//	libgexf::UndirectedGraph& graph = gexf->getUndirectedGraph();

	graph.addNode(Utils::convert2str(node->getID()));

	libgexf::Data& data = gexf->getData();
	data.setNodeLabel(Utils::convert2str(node->getID()), Utils::convert2str(node->getType()->getDemand()));

	if(!node->isRoot()) {
		initNetworkLink(node->toFather(), gexf);
	}

	for ( size_t i = 0; i < node->getChildrenCount(); ++i ) {
		initFaciltyNode(node->toChild(i)->getDestination(), gexf);
	}
}

void FlowConnectionsGexfGen::initNetworkLink(NetworkLink* network, libgexf::GEXF* gexf) {
	if(gexf == NULL) {
		cout << "Problem NULL Pointer with gexf" << endl;
		return;
	}

	libgexf::DirectedGraph& graph = gexf->getDirectedGraph();
//	libgexf::UndirectedGraph& graph = gexf->getUndirectedGraph();

	string idEdge(Utils::convert2str(network->getOrigin()->getID()) + "-" + Utils::convert2str(network->getDestination()->getID()));
	graph.addEdge(idEdge,
					Utils::convert2str(network->getOrigin()->getID()),
					Utils::convert2str(network->getDestination()->getID()));

	libgexf::Data& data = gexf->getData();
	data.setEdgeLabel(idEdge, Utils::convert2str(network->getBandwidth()));

}
