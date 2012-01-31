/*! \file dynamicgraph.h
    \author sebastien heymann
    \date 2 novembre 2009, 18:52
    \version 0.1
 */

#ifndef _DYNAMICGRAPH_H
#define	_DYNAMICGRAPH_H

#include "graph.h"
#include <map>
#include <string>

namespace libgexf {

    /**
     * Class DynamicGraph
     */
    class DynamicGraph {
    public:
        DynamicGraph();
        DynamicGraph(const DynamicGraph& orig);
        virtual ~DynamicGraph();

        bool hasNodeStart(const libgexf::t_id node_id) const;
        bool hasEdgeStart(const libgexf::t_id edge_id) const;

        bool hasNodeEnd(const libgexf::t_id node_id) const;
        bool hasEdgeEnd(const libgexf::t_id edge_id) const;

        std::string getNodeStart(const libgexf::t_id node_id) const;
        std::string getEdgeStart(const libgexf::t_id edge_id) const;

        std::string getNodeEnd(const libgexf::t_id node_id) const;
        std::string getEdgeEnd(const libgexf::t_id edge_id) const;

        void setNodeStart(const libgexf::t_id node_id, const std::string& date);
        void setEdgeStart(const libgexf::t_id edge_id, const std::string& date);

        void setNodeEnd(const libgexf::t_id node_id, const std::string& date);
        void setEdgeEnd(const libgexf::t_id edge_id, const std::string& date);

        
        // DynamicGraph is implemented in terms of a Graph

        unsigned int getDegree(const libgexf::t_id node_id, const std::string& date) const; //TODO impl getDegree
        // libgexf::NodeIter* getNodes(const std::string& date) const;
        // libgexf::NodeIter* getEdges(const std::string& date) const;
        // std::vector<libgexf::t_id> getNeighbors(const libgexf::t_id node_id, const std::string& date) const;
        // unsigned int getNodeCount(const std::string& date) const;
        // unsigned int getEdgeCount(const std::string& date) const;

        void addNode(const libgexf::t_id id) { _graph.addNode(id); }
        void addEdge(const libgexf::t_id id, const libgexf::t_id source_id, const libgexf::t_id target_id, const float weight=1.0, const libgexf::t_edge_type type=EDGE_UNDEF)
        {
            _graph.addEdge(id, source_id, target_id, weight, type);
        }

        void removeNode(const libgexf::t_id id);
        void removeEdge(const libgexf::t_id source_id, const libgexf::t_id target_id);
        bool containsNode(const libgexf::t_id id) const { return _graph.containsNode(id); }
        bool containsEdge(const libgexf::t_id source_id, const libgexf::t_id target_id) const
        {
            return _graph.containsEdge(source_id, target_id);
        }
        t_id getEdge(const libgexf::t_id source_id, const libgexf::t_id target_id) const
        {
            return _graph.getEdge(source_id, target_id);
        }
        libgexf::NodeIter* getNodes() const { return _graph.getNodes(); }
        libgexf::EdgeIter* getEdges() const { return _graph.getEdges(); }
        std::vector<libgexf::t_id> getNeighbors(const libgexf::t_id node_id) const
        {
            return _graph.getNeighbors(node_id);
        }

        /*!
         * Absolute node count over time
         */
        unsigned int getNodeCount() const { return _graph.getNodeCount(); }

        /*!
         * Absolute edge count over time
         */
        unsigned int getEdgeCount() const { return _graph.getEdgeCount(); }

        void clearEdges(const libgexf::t_id node_id);
        void clearEdges();


        void clear();
    private:
        Graph _graph;
        std::map<t_id,std::pair<std::string,std::string> > _nodes_dates; /*!< map<node_id, map<start, end> > */
        std::map<t_id,std::pair<std::string,std::string> > _edges_dates; /*!< map<edge_id, map<start, end> > */
    };

} /* namespace libgexf */

#endif	/* _DYNAMICGRAPH_H */

