/*! \file filewriter.h
    \author sebastien heymann
    \date 8 juillet 2009, 17:58
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

#ifndef _WRITER_H
#define	_WRITER_H

#include "gexf.h"
#include <libxml/xmlwriter.h>
#include <string>

namespace libgexf {

/*! \class FileWriter
    \brief Write a GEXF 1.1 file (Gephi 0.7+).
 */
class FileWriter {
public:
    /*! \var enum ElemType
     *  \brief Possible type of element
     */
    enum ElemType { NODE, EDGE };
public:
    FileWriter();

    /*!
     *  \brief Constructor with init
     *
     *  \param filepath : Path to the written file
     *  \param gexf : reference to a GEXF object
     */
    FileWriter(const std::string& filepath, GEXF* gexf);

    /*!
     *  \brief Copy constructor
     */
    FileWriter(const FileWriter& orig);
    
    virtual ~FileWriter();


    /*!
     *  \brief Get a duplicated instance of the internal GEXF data
     *
     *  \return GEXF instance
     */
    libgexf::GEXF getGEXFCopy();

    /*!
     *  \brief Initialize the file writer
     *
     *  \param filepath : Path to the GEXF file
     *  \param gexf : reference to a GEXF object
     */
    void init(const std::string& filepath, libgexf::GEXF* gexf);

    /*!
     *  \brief Write the file
     *
     */
    void write();
private:
    void writeGexfNode(xmlTextWriterPtr writer);
    void writeMetaNode(xmlTextWriterPtr writer);

    void writeGraphNode(xmlTextWriterPtr writer);
    void writeNodesNode(xmlTextWriterPtr writer);
    void writeNodeNode(xmlTextWriterPtr writer, const std::string& node_id, const std::string& label="");
    void writeEdgesNode(xmlTextWriterPtr writer);
    void writeEdgeNode(xmlTextWriterPtr writer, const std::string& edge_id, const std::string& source_id, const std::string& target_id, const std::string& cardinal="1", const std::string& type="undirected");

    void writeAttributesNode(xmlTextWriterPtr writer, const std::string& element_class);
    void writeAttributeNode(xmlTextWriterPtr writer, const std::string& id, const std::string& title, const std::string& type, const std::string& default_value="", const std::string& options="");
    void writeAttributeDefaultNode(xmlTextWriterPtr writer, const std::string& default_value);
    void writeAttvaluesNode(xmlTextWriterPtr writer, const ElemType type, const std::string& id);
    void writeAttvalueNode(xmlTextWriterPtr writer, const std::string& attribute_id, const std::string& value);
private:
    GEXF* _gexf;
    std::string _filepath;
    static const char* _ENCODING;
};


} /* namespace libgexf */

#endif	/* _WRITER_H */

