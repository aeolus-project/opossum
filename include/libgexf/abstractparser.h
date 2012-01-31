/*! \file abstractparser.h
    \author sebastien heymann
    \date 17 juillet 2009, 15:02
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

#ifndef _ABSTRACTPARSER_H
#define	_ABSTRACTPARSER_H

#include "gexf.h"
#include <libxml/xmlreader.h>

namespace libgexf {

/*! \class AbstractParser
    \brief Parser Interface
 */
class AbstractParser {
public:
    /*!
     *  \brief Bind itself to a GEXF instance
     *
     *  \param gexf : Reference to a GEXF object
     */
    virtual void bind(libgexf::GEXF* gexf) = 0;

    /*!
     *  \brief Start a process on an XML element
     *
     *  Call the right processing method.
     *
     *  \param reader : Reference to the libxml TextReader instance
     *  \param name : Name of the XML element to process
     */
    virtual void processNode(xmlTextReaderPtr reader, const xmlChar* name) = 0;
protected:
    libgexf::t_id getIdAttribute(xmlTextReaderPtr reader, const char* const name);
    std::string getStringAttribute(xmlTextReaderPtr reader, const char* const name);
    std::string getStringAttributeNs(xmlTextReaderPtr reader, const char* const name, const char* const namespaceURI);
    unsigned int getUnsignedIntAttribute(xmlTextReaderPtr reader, const char* const name);
    float getFloatAttribute(xmlTextReaderPtr reader, const char* const name);
};

}

#endif	/* _ABSTRACTPARSER_H */

