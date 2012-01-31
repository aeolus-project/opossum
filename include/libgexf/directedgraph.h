/*! \file directedgraph.h
    \author sebastien heymann
    \date 8 juin 2009, 10:21
    \version 0.1
 */

/*
# Copyright (c) <2009> <Sebastien Heymann>
#
# Permission is hereby granted, free of charge, to any person obtaining a copy
# of this software and associated documentation files (the "Software"), to deal
# in the Software without restriction, including without limitation the rights
# to use, copy, modify, merge, publish, distribute, sublicense, and/or sell
# copies of the Software, and to permit persons to whom the Software is
# furnished to do so, subject to the following conditions:
#
# The above copyright notice and this permission notice shall be included in
# all copies or substantial portions of the Software.
#
# THE SOFTWARE IS PROVIDED "AS IS", WITHOUT WARRANTY OF ANY KIND, EXPRESS OR
# IMPLIED, INCLUDING BUT NOT LIMITED TO THE WARRANTIES OF MERCHANTABILITY,
# FITNESS FOR A PARTICULAR PURPOSE AND NONINFRINGEMENT. IN NO EVENT SHALL THE
# AUTHORS OR COPYRIGHT HOLDERS BE LIABLE FOR ANY CLAIM, DAMAGES OR OTHER
# LIABILITY, WHETHER IN AN ACTION OF CONTRACT, TORT OR OTHERWISE, ARISING FROM,
# OUT OF OR IN CONNECTION WITH THE SOFTWARE OR THE USE OR OTHER DEALINGS IN
# THE SOFTWARE.
*/

#ifndef _DIRECTEDGRAPH_H
#define	_DIRECTEDGRAPH_H

#include "graph.h"
#include <vector>

namespace libgexf {

/*! \class DirectedGraph
    \brief Interpretation of the topology structure as a directed graph.
 */
class DirectedGraph: public Graph {
public:
    DirectedGraph();

    /*!
     *  \brief Copy constructor
     */
    DirectedGraph(const DirectedGraph& orig);
    
    virtual ~DirectedGraph();

    /*!
     *  \brief Remove incoming edges from a node
     *
     *  \param target_id : node ID
     */
    void removeInEdges(const libgexf::t_id target_id);

    /*!
     *  \brief Remove outgoing edges from a node
     *
     *  \param source_id : node ID
     */
    void removeOutEdges(const libgexf::t_id source_id);

    /*!
     *  \brief Get incoming edges from a node
     *
     *  \param node_id : node ID
     *  \return Vector of IDs of incoming edges
     */
    std::vector<libgexf::t_id> getInEdges(const libgexf::t_id node_id) const;

    /*!
     *  \brief Get outgoing edges from a node
     *
     *  \param node_id : node ID
     *  \return Vector of IDs of outgoing edges
     */
    std::vector<libgexf::t_id> getOutEdges(const libgexf::t_id node_id) const;

    /*!
     *  \brief Get node successors
     *
     *  \param node_id : node ID
     *  \return Vector of node IDs
     */
    std::vector<libgexf::t_id> getSuccessors(const libgexf::t_id node_id) const;

    /*!
     *  \brief Get node predecessors
     *
     *  \param node_id : node ID
     *  \return Vector of node IDs
     */
    std::vector<libgexf::t_id> getPredecessors(const libgexf::t_id node_id) const;


    /*!
     *  \brief Get indegree value
     *
     *  \param node_id : node ID
     *  \return indegree value
     */
    unsigned int getInDegree(const libgexf::t_id node_id) const;

    /*!
     *  \brief Get outdegree value
     *
     *  \param node_id : node ID
     *  \return outdegree value
     */
    unsigned int getOutDegree(const libgexf::t_id node_id) const;


    /*!
     *  \brief Test a possible successor
     *
     *  Check if the successor_id is a successor of the node node_id.
     *
     *  \param node_id : node ID
     *  \param successor_id : node ID of the tested successor
     *  \return true if successor_id is a successor of node_id
     */
    bool isSuccessor(const libgexf::t_id node_id, const libgexf::t_id successor_id) const;

    /*!
     *  \brief Test a possible predecessor
     *
     *  Check if the predecessor_id is a predecessor of the node node_id.
     *
     *  \param node_id : node ID
     *  \param predecessor_id : node ID of the tested predecessor
     *  \return true if predecessor_id is a predecessor of node_id
     */
    bool isPredecessor(const libgexf::t_id node_id, const libgexf::t_id predecessor_id) const;
private:
    DirectedGraph& operator=(const DirectedGraph& orig);
};

}

#endif	/* _DIRECTEDGRAPH_H */

