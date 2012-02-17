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

#ifndef GEXFGEN_HPP_
#define GEXFGEN_HPP_

#include <libgexf/libgexf.h>
#include "network.hpp"

using namespace std;

/*
For the 1st graph :
Attributs Node : MaxNumberOfServers,
				 Demands,
				 NumberOfConnexions,
				 NumberOfLocalConnexions

Attribut Link : Bandwidth,
				Reliability
------------------------------------------------------------------------------------
For the 2nd graph :
Attributs Node : MaxNumberOfServers,
				 Demands,
				 NumberOfConnexions,
				 NumberOfLocalConnexions

Attribut Link : NumberOfConnexionsOnEdgeY //nb of connexions on the link (yij)
				CumulativeBandwidth, //bandwidth on the cumulative link bij
------------------------------------------------------------------------------------
For the 3nd graph :
Attributs Node : MaxNumberOfServers,
				 Demands,
				 NumberOfConnexions,
				 NumberOfLocalConnexions

Attribut Link : NumberOfConnexionsOnEdgeZ //zij nb of connexions of facility from i to j with the link (i,j)

*/

enum NodeAttribute {
	MaxNumberOfServers,
	Demands,
	NumberOfConnexions,
	NumberOfLocalConnexions
};

enum EdgeAttribute {
	Bandwidth,
	Reliability,
	NumberOfConnexionsOnEdgeY, //nb of connexions on the link (yij)
	CumulativeBandwidth, //bandwidth on the cumulative link bij
	NumberOfConnexionsOnEdgeZ //zij nb of connexions of facility from i to j with the link (i,j)
};

class AbstractGexfGen {
protected:
	libgexf::GEXF* _gexf;
	FacilityNode* _root;
	virtual void initFacilityNode(FacilityNode* node) = 0;
	virtual void initNetworkLink(NetworkLink* network) = 0;
	void initGexf();

public:
	AbstractGexfGen(FacilityNode* root);
	void writeToFile(string pathFile);

}; 

class InstanceGexfGen : public AbstractGexfGen {
protected:
	void initFacilityNode(FacilityNode* node);
	void initNetworkLink(NetworkLink* network);

public:
	InstanceGexfGen(FacilityNode* root);
};


class FlowConnectionsGexfGen : public AbstractGexfGen {
protected:
	void initFacilityNode(FacilityNode* node);
	void initNetworkLink(NetworkLink* network);

public:
	FlowConnectionsGexfGen(FacilityNode* root);
};

#endif //GEXFGEN_HPP_
