/*! \file gexf.h
    \author sebastien heymann
    \date 17 avril 2009, 17:28
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

#ifndef _GEXF_H
#define	_GEXF_H

#include <iostream>
#include "graph.h"
#include "undirectedgraph.h"
#include "directedgraph.h"
#include "data.h"
#include "metadata.h"
#include "memoryvalidator.h"

namespace libgexf {

    class MemoryValidator;

    /*! \class GEXF
        \brief GEXF class, just a container.

        \li graph topology
        \li data attributes
        \li hierarchy (currently not available)
        \li viz data (currently not available)
        \li dynamics (currently not available)
     */
    class GEXF {
    public:
        GEXF();

        /*!
         *  \brief Copy constructor
         */
        GEXF(const GEXF& orig);
        
        virtual ~GEXF();


        /*!
         *  \brief Get an undirected graph instance
         *
         *  \return Undirected graph
         */
        libgexf::UndirectedGraph& getUndirectedGraph();

        /*!
         *  \brief Get a directed graph instance
         *
         *  \return Directed graph
         */
        libgexf::DirectedGraph& getDirectedGraph();

        /*!
         *  \brief Get associated data instance
         *
         *  \return graph data
         */
        libgexf::Data& getData();

        /*!
         *  \brief Get associated meta data instance
         *
         *  \return graph meta data
         */
        libgexf::MetaData& getMetaData();


        /*!
         *  \brief Change the type of graph
         *
         *  \param t : Type of graph (directed, undirected or mixed)
         */
        void setGraphType(libgexf::t_graph t);

        /*!
         *  \brief Get the type of graph
         *
         *  \return Type of graph (directed, undirected or mixed)
         */
        libgexf::t_graph getGraphType();

        /*!
         *  \brief Initialize the graph mode
         *
         *  \param mode : Mode of graph (static or dynamic)
         */
        void initGraphMode(const std::string& mode);

        /*!
         *  \brief Get the graph mode
         *
         *  \return Mode of graph (static or dynamic)
         */
        std::string getGraphMode();


        /*!
         *  \brief Check the data correctness
         *
         *  \li verify if each node has a label
         *  \li verify if each attvalue has a value or a defaultvalue
         *
         * \return true if it is ok, false otherwise.
         */
        bool checkIntegrity();

        libgexf::Graph _graph;  /*!< Topology structure */
        libgexf::t_graph _type;  /*!< Default edge type */
        std::string _mode;  /*!< Graph mode */
        libgexf::Data _data;  /*!< Associated data and attributes on nodes and edges */
        libgexf::MetaData _meta;  /*!< Associated meta data */
    private:
        friend std::ostream& operator<<(std::ostream& os, const GEXF& o);
        friend class MemoryValidator; /*!< MemoryValidator */
    };

} /* namespace libgexf */

#endif	/* _GEXF_H */

