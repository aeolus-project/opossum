/*! \file conv.h
    \author sebastien heymann
    \date 15 juillet 2009, 18:36
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

#ifndef _CONV_H
#define	_CONV_H

#include "typedefs.h"
#include "data.h"
#include "gexfparser.h"
#include <string>
#include <set>
#include <libxml/xmlstring.h>

namespace libgexf {

    class Data;
    class GexfParser;
    class LegacyParser;

    /*! \class Conv
        \brief Utility static class for transforming data.
     */
    class Conv {
    public:
        Conv();
        Conv(const Conv& orig);
        virtual ~Conv();

        static libgexf::t_id xmlCharToId(const xmlChar* str);
        static libgexf::t_id strToId(const std::string& str);
        static std::string xmlCharToStr(const xmlChar* str);
        static unsigned int xmlCharToUnsignedInt(const xmlChar* str);
        static float xmlCharToFloat(const xmlChar* str);
        static std::string idToStr(const libgexf::t_id id);
        static std::string unsignedIntToStr(const unsigned int i);
        static std::string floatToStr(const float f);
        static unsigned int strToUnsignedInt(const std::string& str);
        static std::string edgeTypeToStr(const libgexf::t_edge_type t);
        static std::string attrTypeToStr(const libgexf::t_attr_type t);
        static bool isBoolean(const std::string& str);
        static bool isDouble(const std::string& str);
        static bool isInteger(const std::string& str);
        static bool isFloat(const std::string& str);
        static bool isLong(const std::string& str);
        static bool isAnyURI(const std::string& str);
        static std::set<std::string> tokenizer(const std::string& delimiter, const std::string& str);
    private:
        virtual void iAmAbstract() = 0;

        /*static libgexf::t_edge_type strToEdgeType(const std::string& str);*/
        static libgexf::t_attr_type strToAttrType(const std::string& str);
        friend class Data; /*!< Data */
        friend class GexfParser; /*!< GexfParser */
        friend class LegacyParser; /*!< LegacyParser */
    };

} /* namespace libgexf */

#endif	/* _CONV_H */

