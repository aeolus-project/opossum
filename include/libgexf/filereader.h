/*! \file filereader.h
    \author sebastien heymann
    \date 19 juin 2009, 12:37
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

#ifndef _FILEREADER_H
#define	_FILEREADER_H

#include "gexf.h"
#include "abstractparser.h"


namespace libgexf {

/*! \class FileReader
    \brief Read a GEXF file.
 */
class FileReader {
public:
    /*! \var enum Version
     *  \brief Possible version number of the GEXF format
     */
    enum Version { _1_0, _1_1};
public:
    FileReader();

    /*!
     *  \brief Constructor with init
     *
     *  \param filepath : Path to the written file
     *  \param v : version number of the GEXF format
     */
    explicit FileReader(const std::string& filepath, const Version v=_1_1);

    /*!
     *  \brief Copy constructor
     */
    FileReader(const FileReader& orig);

    virtual ~FileReader();


    /*!
     *  \brief Get a duplicated instance of the internal GEXF data
     *
     *  \return GEXF instance
     */
    libgexf::GEXF getGEXFCopy();

    /*!
     *  \brief Initialize the file reader
     *
     *  \param filepath : Path to the GEXF file
     *  \param v : version number of the GEXF format
     */
    void init(const std::string& filepath, const Version v=_1_1);

    /*!
     *  \brief Read the given file in one pass
     *
     */
    void slurp();

    //int relaxNGValidate(const std::string schema);
private:
    void createParser();
    void streamFile();
private:
    GEXF* _gexf;
    AbstractParser* _parser;
    std::string _filepath;
    Version _v;
};

} /* namespace libgexf */

#endif	/* _FILEREADER_H */
