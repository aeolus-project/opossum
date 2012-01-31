/*! \file libgexf.h
    \author sebastien heymann
    \date 22 juin 2009, 16:45
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

#ifndef _LIBGEXF_H
#define	_LIBGEXF_H

#include <libgexf/typedefs.h>
#include <libgexf/conv.h>
#include <libgexf/exceptions.h>
#include <libgexf/gexf.h>
#include <libgexf/graph.h>
#include <libgexf/dynamicgraph.h>
#include <libgexf/metadata.h>
#include <libgexf/data.h>
#include <libgexf/abstractiter.h>
#include <libgexf/nodeiter.h>
#include <libgexf/edgeiter.h>
#include <libgexf/directedgraph.h>
#include <libgexf/undirectedgraph.h>
#include <libgexf/abstractparser.h>
#include <libgexf/filereader.h>
#include <libgexf/filewriter.h>
#include <libgexf/legacywriter.h>
#include <libgexf/rngvalidator.h>
#include <libgexf/schemavalidator.h>
#include <libgexf/memoryvalidator.h>

#endif	/* _LIBGEXF_H */

