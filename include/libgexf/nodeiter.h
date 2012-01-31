/*! \file nodeiter.h
    \author sebastien heymann
    \date 9 juillet 2009, 15:48
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

#ifndef _NodeIter_H
#define	_NodeIter_H

#include "typedefs.h"
#include "graph.h"
#include "abstractiter.h"

#include <set>

namespace libgexf {

class Graph;

/*! \class NodeIter
    \brief Iterator on nodes.
 */
class NodeIter: public AbstractIter {
public:
    /*!
     *  \brief Constructor
     *
     *  \param g : Reference to the Graph object
     */
    explicit NodeIter(const libgexf::Graph* g);
    virtual ~NodeIter();

    NodeIter* begin();
    bool hasNext() const;

    /*!
     *  \brief Iterate
     *
     *  Get next element in collection.
     *
     *  \return The node ID.
     */
    libgexf::t_id next();
private:
    const Graph* _graph;
    std::set<t_id>::const_iterator _it;
    unsigned int _cpt;
    unsigned int _nb_items;
};

} /* namespace libgexf */

#endif	/* _NodeIter_H */

