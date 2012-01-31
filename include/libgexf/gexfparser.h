/*! \file gexfparser.h
    \author sebastien heymann
    \date 22 juin 2009, 17:20
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

#ifndef _GEXF_PARSER_H
#define	_GEXF_PARSER_H

#include "gexf.h"
#include "typedefs.h"
#include "abstractparser.h"
#include <libxml/xmlreader.h>
#include <string>

namespace libgexf {

class Reader;

/*! \class GexfParser
    \brief Parse a GEXF file.
 */
class GexfParser: public AbstractParser {
public:
    GexfParser();

    /*!
     *  \brief Copy constructor
     */
    GexfParser(const GexfParser& orig);
    
    virtual ~GexfParser();

    void bind(libgexf::GEXF* gexf);
    void processNode(xmlTextReaderPtr reader, const xmlChar* const name);
private:
    void processGEXFNode(xmlTextReaderPtr reader);
    void processMetaNode(xmlTextReaderPtr reader);
    void processCreatorNode(xmlTextReaderPtr reader);
    void processDescriptionNode(xmlTextReaderPtr reader);

    void processGraphNode(xmlTextReaderPtr reader);
    void processNodesNode(xmlTextReaderPtr reader);
    void processNodeNode(xmlTextReaderPtr reader);
    void processEdgesNode(xmlTextReaderPtr reader);
    void processEdgeNode(xmlTextReaderPtr reader);

    void processAttributesNode(xmlTextReaderPtr reader);
    void processAttributeNode(xmlTextReaderPtr reader);
    void processAttributeDefaultNode(xmlTextReaderPtr reader);
    void processAttributeOptionsNode(xmlTextReaderPtr reader);
    void processAttvaluesNode(xmlTextReaderPtr reader);
    void processAttvalueNode(xmlTextReaderPtr reader);
    
    bool isProcessableNode(xmlTextReaderPtr reader);
private:
    enum ElemType { NODE, EDGE, ATTR_NODE, ATTR_EDGE };
    GEXF* _gexf;
    ElemType _last_node_type;
    libgexf::t_id _last_id;
};

} /* namespace libgexf */

#endif	/* _GEXF_PARSER_H */

