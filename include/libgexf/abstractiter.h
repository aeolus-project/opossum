/*! \file abstractiter.h
    \author sebastien heymann
    \date 20 juillet 2009, 11:44
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

#ifndef _ABSTRACTITER_H
#define	_ABSTRACTITER_H

namespace libgexf {

/*! \class AbstractIter
    \brief Iterator Interface
 */
class AbstractIter {
public:
    /*!
     *  \brief Init
     *
     *  Initialize (or re-initialize) the iterator.
     *
     *  \return an instance of the iterator
     */
    virtual AbstractIter* begin() = 0;

    /*!
     *  \brief Test next element in collection.
     *
     *  Verify if another element exists.
     *
     *  \return true if another element exists, false otherwise.
     */
    virtual bool hasNext() const = 0;

    /*!
     *  \brief Iterate
     *
     *  Get next element in collection.
     *
     *  \return The element ID.
     */
    virtual libgexf::t_id next() = 0;
};

}

#endif	/* _ABSTRACTITER_H */

