/*! \file legacyparser.h
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

#ifndef _LEGACY_PARSER_H
#define	_LEGACY_PARSER_H

#include "gexf.h"
#include "typedefs.h"
#include "abstractparser.h"
#include <libxml/xmlreader.h>
#include <string>

namespace libgexf {

class Reader;

/*! \class LegacyParser
    \brief Parse an old GEXF file (1.0).
 */
class LegacyParser: public AbstractParser {
public:
    LegacyParser();

    /*!
     *  \brief Copy constructor
     */
    LegacyParser(const LegacyParser& orig);
    
    virtual ~LegacyParser();

    void bind(libgexf::GEXF* gexf);
    void processNode(xmlTextReaderPtr reader, const xmlChar* name);
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
    void processAttvaluesNode(xmlTextReaderPtr reader);
    void processAttvalueNode(xmlTextReaderPtr reader);
    
    bool isProcessableNode(xmlTextReaderPtr reader);
private:
    enum ElemType { NODE, EDGE, ATTR_NODE, ATTR_EDGE };
    GEXF* _gexf;
    ElemType _last_node_type;
    t_id _last_id;
};

} /* namespace libgexf */

#endif	/* _LEGACY_PARSER_H */

