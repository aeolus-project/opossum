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

using namespace Utils;

AbstractGexfGen::AbstractGexfGen(FacilityNode* root) : _root(root), _gexf(NULL) {
	_gexf = new libgexf::GEXF();
}

void AbstractGexfGen::initGexf() {

	initFacilityNode(_root);

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

	this->initGexf();

	// write gexf file
    libgexf::FileWriter *writer = new libgexf::FileWriter();
    writer->init(pathFile, _gexf);
    writer->write();
}

//TO DO : Mettre la reliability, bandwith, demande, capacity server
// Générer les instances et pouvoirles visualiser
//Taille du noeud dépend de la demande
//Couleur pour le nombre de serveurs
//Taille du noeud en fonction de la realibility
//Faire les solutions à la main

InstanceGexfGen::InstanceGexfGen(FacilityNode* root) : AbstractGexfGen(root) {

	libgexf::Data& data = _gexf->getData();
	//NodeAttributes
	data.addNodeAttributeColumn(convert2str(MaxNumberOfServers), "MaxNumberOfServers",  "INTEGER");
	data.addNodeAttributeColumn(convert2str(Demands), "Demands", "INTEGER");
	data.addNodeAttributeColumn(convert2str(NumberOfConnexions), "NumberOfConnexions", "INTEGER");
	data.addNodeAttributeColumn(convert2str(NumberOfLocalConnexions), "NumberOfLocalConnexions", "INTEGER");

	data.setNodeAttributeDefault(convert2str(MaxNumberOfServers), "0");
	data.setNodeAttributeDefault(convert2str(Demands), "0");
	data.setNodeAttributeDefault(convert2str(NumberOfConnexions), "0");
	data.setNodeAttributeDefault(convert2str(NumberOfLocalConnexions), "0");

	//EdgeAttributes
	data.addEdgeAttributeColumn(convert2str(Bandwidth), "Bandwidth", "DOUBLE");
	data.addEdgeAttributeColumn(convert2str(Reliability), "Reliability", "INTEGER");

	data.setEdgeAttributeDefault(convert2str(Bandwidth), "0.0");
	data.setEdgeAttributeDefault(convert2str(Reliability), "0");

}

void InstanceGexfGen::initFacilityNode(FacilityNode* node) {

	libgexf::DirectedGraph& graph = _gexf->getDirectedGraph();
//	libgexf::UndirectedGraph& graph = _gexf->getUndirectedGraph();

	graph.addNode(convert2str(node->getID()));

	libgexf::Data& data = _gexf->getData();
	data.setNodeLabel(convert2str(node->getID()), convert2str(node->getID()));

	data.setNodeValue(convert2str(node->getID()), convert2str(MaxNumberOfServers), convert2str(node->getType()->getMaxServerCapacities()));
	data.setNodeValue(convert2str(node->getID()), convert2str(Demands), convert2str(node->getType()->getDemand()));
	data.setNodeValue(convert2str(node->getID()), convert2str(NumberOfConnexions), "10");
	data.setNodeValue(convert2str(node->getID()), convert2str(NumberOfLocalConnexions), "10");

	if(!node->isRoot()) {
		initNetworkLink(node->toFather());
	}

	for ( size_t i = 0; i < node->getChildrenCount(); ++i ) {
		initFacilityNode(node->toChild(i)->getDestination());
	}
}

void InstanceGexfGen::initNetworkLink(NetworkLink* network) {

	libgexf::DirectedGraph& graph = _gexf->getDirectedGraph();
//	libgexf::UndirectedGraph& graph = _gexf->getUndirectedGraph();

	float reliable = 1.0;
	if(network->isReliable()) {
		reliable = 2.0;
	}

	string idEdge(convert2str(network->getOrigin()->getID()) + "-" + convert2str(network->getDestination()->getID()));
	graph.addEdge(idEdge,
					convert2str(network->getOrigin()->getID()),
					convert2str(network->getDestination()->getID()),
					reliable);

	libgexf::Data& data = _gexf->getData();
	data.setEdgeLabel(idEdge, convert2str(network->getBandwidth()));
	data.setEdgeValue(convert2str(idEdge), convert2str(Bandwidth), convert2str(network->getBandwidth()));
	data.setEdgeValue(convert2str(idEdge), convert2str(Reliability), convert2str(network->isReliable()));
}

FlowConnectionsGexfGen::FlowConnectionsGexfGen(FacilityNode* root) : AbstractGexfGen(root) {
	libgexf::Data& data = _gexf->getData();
	//NodeAttributes
	data.addNodeAttributeColumn(convert2str(MaxNumberOfServers), "MaxNumberOfServers", "INTEGER");
	data.addNodeAttributeColumn(convert2str(Demands), "Demands", "INTEGER");
	data.addNodeAttributeColumn(convert2str(NumberOfConnexions), "NumberOfConnexions", "INTEGER");
	data.addNodeAttributeColumn(convert2str(NumberOfLocalConnexions), "NumberOfLocalConnexions", "INTEGER");

	data.setNodeAttributeDefault(convert2str(MaxNumberOfServers), "0");
	data.setNodeAttributeDefault(convert2str(Demands), "0");
	data.setNodeAttributeDefault(convert2str(NumberOfConnexions), "0");
	data.setNodeAttributeDefault(convert2str(NumberOfLocalConnexions), "0");


	//EdgeAttributes
	data.addEdgeAttributeColumn(convert2str(NumberOfConnexionsOnEdgeY), "NumberOfConnexionsOnEdgeY", "INTEGER");
	data.addEdgeAttributeColumn(convert2str(CumulativeBandwidth), "CumulativeBandwidth", "DOUBLE");

	data.setEdgeAttributeDefault(convert2str(NumberOfConnexionsOnEdgeY), "0");
	data.setEdgeAttributeDefault(convert2str(CumulativeBandwidth), "0.0");


}

void FlowConnectionsGexfGen::initFacilityNode(FacilityNode* node) {


	libgexf::DirectedGraph& graph = _gexf->getDirectedGraph();
//	libgexf::UndirectedGraph& graph = _gexf->getUndirectedGraph();

	graph.addNode(convert2str(node->getID()));

	libgexf::Data& data = _gexf->getData();
	data.setNodeLabel(convert2str(node->getID()), convert2str(node->getID()));

	data.setNodeValue(convert2str(node->getID()), convert2str(MaxNumberOfServers), convert2str(node->getType()->getMaxServerCapacities()));
	data.setNodeValue(convert2str(node->getID()), convert2str(Demands), convert2str(node->getType()->getDemand()));
	data.setNodeValue(convert2str(node->getID()), convert2str(NumberOfConnexions), "10");
	data.setNodeValue(convert2str(node->getID()), convert2str(NumberOfLocalConnexions), "10");


	if(!node->isRoot()) {
		initNetworkLink(node->toFather());
	}

	for ( size_t i = 0; i < node->getChildrenCount(); ++i ) {
		initFacilityNode(node->toChild(i)->getDestination());
	}
}

void FlowConnectionsGexfGen::initNetworkLink(NetworkLink* network) {

	libgexf::DirectedGraph& graph = _gexf->getDirectedGraph();
//	libgexf::UndirectedGraph& graph = _gexf->getUndirectedGraph();

	float reliable = 1.0;
	if(network->isReliable()) {
		reliable = 2.0;
	}

	string idEdge(convert2str(network->getOrigin()->getID()) + "-" + convert2str(network->getDestination()->getID()));
	graph.addEdge(idEdge,
					convert2str(network->getOrigin()->getID()),
					convert2str(network->getDestination()->getID()),
					reliable);

	libgexf::Data& data = _gexf->getData();
	data.setEdgeLabel(idEdge, convert2str(network->getBandwidth()));

	data.setEdgeValue(convert2str(idEdge), convert2str(NumberOfConnexionsOnEdgeY), "10");
	data.setEdgeValue(convert2str(idEdge), convert2str(CumulativeBandwidth), "100.0");
}
