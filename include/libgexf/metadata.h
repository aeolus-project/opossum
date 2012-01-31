/*! \file metadata.h
    \author sebastien heymann
    \date 30 juin 2009, 14:14
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

#ifndef _METADATA_H
#define	_METADATA_H

#include <string>

namespace libgexf {

/*! \class MetaData
    \brief Associated meta data and attributes on the graph.
 */
class MetaData {
public:
    MetaData();

    /*!
     *  \brief Copy constructor
     */
    MetaData(const MetaData& orig);
    
    virtual ~MetaData();

    /*!
     *  \brief Get the GEXF version
     */
    std::string getVersion() const;

    /*!
     *  \brief Get the XMLNS
     */
    std::string getXmlns() const;

    /*!
     *  \brief Get the XMLNS:XSI
     */
    std::string getXsi() const;

    /*!
     *  \brief Get the XML schema
     */
    std::string getSchema() const;

    /*!
     *  \brief Get the variant
     */
    std::string getVariant() const;

    /*!
     *  \brief Get the creator
     */
    std::string getCreator() const;

    /*!
     *  \brief Get the description
     */
    std::string getDescription() const;

    /*!
     *  \brief Get the keywords
     */
    std::string getKeywords() const;

    /*!
     *  \brief Get the last modified date
     */
    std::string getLastModifiedDate() const;


    
    /*!
     *  \brief Set the GEXF version
     */
    void setVersion(const std::string& version);

    /*!
     *  \brief Set the XMLNS
     */
    void setXmlns(const std::string& xmlns);

    /*!
     *  \brief Set the XMLNS:XSI
     */
    void setXsi(const std::string& xsi);

    /*!
     *  \brief Set the XML schema
     */
    void setSchema(const std::string& schema);

    /*!
     *  \brief Set the variant
     */
    void setVariant(const std::string& variant);

    /*!
     *  \brief Set the creator
     */
    void setCreator(const std::string& creator);

    /*!
     *  \brief Set the description
     */
    void setDescription(const std::string& description);

    /*!
     *  \brief Set the keywords
     */
    void setKeywords(const std::string& keywords);

    /*!
     *  \brief Set the last modified date
     */
    void setLastModifiedDate(const std::string& lastmodifieddate);
private:
    void init();
private:
    std::string _version;
    std::string _xmlns;
    std::string _xsi;
    std::string _schema;
    std::string _variant;
    std::string _creator;
    std::string _description;
    std::string _keywords;
    std::string _lastmodifieddate;
private:
    friend std::ostream& operator<<(std::ostream& os, const MetaData& o);
};

}

#endif	/* _METADATA_H */

