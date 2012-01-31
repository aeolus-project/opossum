/*! \file rngvalidator.h
    \author sebastien heymann
    \date 30 juillet 2009, 23:12
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

#ifndef _RNG_VALIDATOR_H
#define	_RNG_VALIDATOR_H

#include <libxml/xmlreader.h>
#include <libxml/relaxng.h>
#include <string>

namespace libgexf {

/*! \class RngValidator
    \brief RelaxNG validator
*/
class RngValidator {
public:
    RngValidator();

    /*!
     *  \brief Copy constructor
     */
    RngValidator(const RngValidator& orig);
    
    virtual ~RngValidator();

    /*!
     *  \brief Validate an XML file based on a RelaxNG Schema
     *
     *  \param xml_file_pathname : path of the XML file to validate
     *  \param rng_file_pathname : path of the RNG file
     * 
     *  \return true if valid, false otherwise
     */
    static bool run(const std::string& xml_file_pathname, const std::string& rng_file_pathname);
private:
    // Don't know what this handles, can't get it to fire.
    static void readerErr(void* arg, const char* const msg, xmlParserSeverities severity, xmlTextReaderLocatorPtr locator);

    // handles well-formedness errors in instance document
    // and handles validity errors in instance doc
    static void structErr(void* userData, xmlErrorPtr error);

    // handles warnings encountered while parsing RNG schema
    static void rngWarn(void* ctx, const char* const msg, xmlParserSeverities severity, xmlTextReaderLocatorPtr locator);

    // handles errors encountered while parsing RNG schema
    static void rngErr(void* ctx, const char* const msg, xmlParserSeverities severity, xmlTextReaderLocatorPtr locator);
    
    virtual void iAmAbstract() = 0;
};

} /* namespace libgexf */

#endif	/* _RNG_VALIDATOR_H */

