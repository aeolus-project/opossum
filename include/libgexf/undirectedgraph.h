/*! \file undirectedgraph.h
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

#ifndef _UNDIRECTEDGRAPH_H
#define	_UNDIRECTEDGRAPH_H

#include "graph.h"

namespace libgexf {

/*! \class UndirectedGraph
    \brief Interpretation of the topology structure as a undirected graph.
 */
class UndirectedGraph: public Graph {
public:
    UndirectedGraph();

    /*!
     *  \brief Copy constructor
     */
    UndirectedGraph(const UndirectedGraph& orig);
    
    virtual ~UndirectedGraph();

    /*!
     *  \brief Merge edges where {source,target} <> {target,source}
     */
    //void mergeSimilarEdges();
private:
    UndirectedGraph& operator=(const UndirectedGraph& orig);
};

}

#endif	/* _UNDIRECTEDGRAPH_H */

