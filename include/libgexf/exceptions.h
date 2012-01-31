/*! \file exceptions.h
    \author sebastien heymann
    \date 4 juin 2009, 10:36
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

#ifndef _EXCEPTIONS_H
#define	_EXCEPTIONS_H


#include <exception>
#include <string>
#include <sstream>
#include "typedefs.h"

namespace libgexf {

    /*! \class ReadLockException
        \brief Exception occuring on a read-lock.
     */
    class ReadLockException : public std::exception {
    public:
        ReadLockException(const std::string& what) throw() { _text = what; };
        virtual ~ReadLockException() throw() {};
        virtual const char* what() const throw() {
            return (const char *)(_text.c_str());
        }
    private:
        std::string _text;
    };

    /*! \class WriteLockException
        \brief Exception occuring on a write-lock.
     */
    class WriteLockException : public std::exception {
    public:
        WriteLockException(const std::string& what) throw() { _text = what; };
        virtual ~WriteLockException() throw() {};
        virtual const char* what() const throw() {
            return (const char *)(_text.c_str());
        }
    private:
        std::string _text;
    };

    /*! \class FileWriterException
        \brief Exception occuring when writing a file
     */
    class FileWriterException : public std::exception {
    public:
        FileWriterException(const std::string& what) throw() { _text = what; };
        virtual ~FileWriterException() throw() {};
        virtual const char* what() const throw() {
            return (const char *)(_text.c_str());
        }
    private:
        std::string _text;
    };

    /*! \class FileReaderException
        \brief Exception occuring when reading a file
     */
    class FileReaderException : public std::exception {
    public:
        FileReaderException(const std::string& what) throw() { _text = what; };
        virtual ~FileReaderException() throw() {};
        virtual const char* what() const throw() {
            return (const char *)(_text.c_str());
        }
    private:
        std::string _text;
    };
}

#endif	/* _EXCEPTIONS_H */

