/*! \file memoryvalidator.h
    \author sebastien heymann
    \date 2 septembre 2009, 14:53
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

#ifndef _MEMORYVALIDATOR_H
#define	_MEMORYVALIDATOR_H

#include "gexf.h"

namespace libgexf {

    class GEXF;

    /*! \class MemoryValidator
        \brief Static class to check data correctness
     */
    class MemoryValidator {
    public:
        MemoryValidator();

        /*!
         *  \brief Copy constructor
         */
        MemoryValidator(const MemoryValidator& orig);
        
        virtual ~MemoryValidator();

        /*!
         *  \brief Check the data correctness
         *
         *  \li verify if each node has a label
         *  \li verify if each attvalue has a value or a defaultvalue
         */
        static bool run(const GEXF& gexf);
    private:
        static bool checkNodeLabels(const GEXF& gexf);
        static bool checkDefaultOptions(const GEXF& gexf);
        static bool checkAttValues(const GEXF& gexf);
        static bool checkAttValueType(const GEXF& gexf, const std::string& value, const libgexf::t_attr_type type, const t_id elem_id, const t_id attr_id="", const bool isNode=true);

        virtual void iAmAbstract() = 0;
    };

} /* namespace libgexf */

#endif	/* _MEMORYVALIDATOR_H */

