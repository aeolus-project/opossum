/*! \file data.h
    \author sebastien heymann
    \date 30 juin 2009, 13:35
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

#ifndef _DATA_H
#define	_DATA_H

#include "typedefs.h"
#include "attributeiter.h"
#include "attvalueiter.h"
#include <string>
#include <map>
#include <set>

namespace libgexf {

class AttributeIter;
class AttValueIter;

/*! \class Data
    \brief Associated data and attributes on nodes and edges.
 */
class Data {
public:
    Data();

    /*!
     *  \brief Copy constructor
     */
    Data(const Data& orig);
    
    virtual ~Data();


    /*!
     *  \brief Get node label
     *
     *  \param node_id : node ID
     */
    std::string getNodeLabel(const libgexf::t_id node_id) const;

    /*!
     *  \brief Check if the node label exists
     *
     *  \param node_id : node ID
     */
    bool hasNodeLabel(const libgexf::t_id node_id) const;

    /*!
     *  \brief Set node label
     *
     *  \param node_id : node ID
     *  \param label : node label
     */
    void setNodeLabel(const libgexf::t_id node_id, const std::string& label);

    
    /*!
     *  \brief Get edge label
     *
     *  \param edge_id : edge ID
     */
    std::string getEdgeLabel(const libgexf::t_id edge_id) const;

    /*!
     *  \brief Check if the edge label exists
     *
     *  \param edge_id : edge ID
     */
    bool hasEdgeLabel(const libgexf::t_id edge_id) const;

    /*!
     *  \brief Set edge label
     *
     *  \param edge_id : edge ID
     *  \param label : edge label
     */
    void setEdgeLabel(const libgexf::t_id edge_id, const std::string& label);


    /*!
     *  \brief Add a node attribute column
     *
     *  \param id : attribute ID
     *  \param title : name of the node attribute
     *  \param type : type of attribute (integer, double, float, boolean, string or list-string)
     */
    void addNodeAttributeColumn(const libgexf::t_id id, const std::string& title, const std::string& type="STRING");

    /*!
     *  \brief Add an edge attribute column
     *
     *  \param id : attribute ID
     *  \param title : name of the edge attribute
     *  \param type : type of attribute (integer, double, float, boolean, string or list-string)
     */
    void addEdgeAttributeColumn(const libgexf::t_id id, const std::string& title, const std::string& type="STRING");


    /*!
     *  \brief Set the default value of a node attribute
     *
     *  \param attr_id : attribute ID
     *  \param default_value : default value
     */
    void setNodeAttributeDefault(const libgexf::t_id attr_id, const std::string& default_value);

    /*!
     *  \brief Set the default value of an edge attribute
     *
     *  \param attr_id : attribute ID
     *  \param default_value : default value
     */
    void setEdgeAttributeDefault(const libgexf::t_id attr_id, const std::string& default_value);


    /*!
     *  \brief Set the options of a node attribute
     *
     *  \param attr_id : attribute ID
     *  \param options : options
     */
    void setNodeAttributeOptions(const libgexf::t_id attr_id, const std::string& options);

    /*!
     *  \brief Set the options of an edge attribute
     *
     *  \param attr_id : attribute ID
     *  \param options : options
     */
    void setEdgeAttributeOptions(const libgexf::t_id attr_id, const std::string& options);


    /*!
     *  \brief Set the node value of an attribute
     *
     *  \param node_id : node ID
     *  \param attr_id : attribute ID
     *  \param value : node value
     */
    void setNodeValue(const libgexf::t_id node_id, const libgexf::t_id attr_id, const std::string& value);
    
    /*!
     *  \brief Set the edge value of an attribute
     *
     *  \param edge_id : edge ID
     *  \param attr_id : attribute ID
     *  \param value : edge value
     */
    void setEdgeValue(const libgexf::t_id edge_id, const libgexf::t_id attr_id, const std::string& value);


    /*!
     *  \brief Get an iterator on the node attribute column
     *
     *  \return Attribute iterator instance
     */
    libgexf::AttributeIter* getNodeAttributeColumn() const;

    /*!
     *  \brief Get an iterator on the edge attribute column
     *
     *  \return Attribute iterator instance
     */
    libgexf::AttributeIter* getEdgeAttributeColumn() const;


    /*!
     *  \brief Get the node attribute value
     *
     *  \param node_id : node ID
     *  \param attr_id : attribute ID
     *  \return node attribute value
     */
    std::string getNodeAttribute(const libgexf::t_id node_id, const libgexf::t_id attr_id) const;

    /*!
     *  \brief Get the edge attribute value
     *
     *  \param edge_id : edge ID
     *  \param attr_id : attribute ID
     *  \return edge attribute value
     */
    std::string getEdgeAttribute(const libgexf::t_id edge_id, const libgexf::t_id attr_id) const;


    /*!
     *  \brief Get an iterator on the node attribute row
     *
     *  \param node_id : node ID
     *  \return iterator instance on attribute values
     */
    libgexf::AttValueIter* getNodeAttributeRow(const libgexf::t_id node_id) const;

    /*!
     *  \brief Get an iterator on the edge attribute row
     *
     *  \param edge_id : edge ID
     *  \return iterator instance on attribute values
     */
    libgexf::AttValueIter* getEdgeAttributeRow(const libgexf::t_id edge_id) const;


    /*!
     *  \brief Get the default value of a node attribute
     *
     *  \param attr_id : attribute ID
     *  \return default value
     */
    std::string getNodeAttributeDefault(const libgexf::t_id attr_id) const;

    
    /*!
     *  \brief Get the options of an edge attribute
     *
     *  \param attr_id : attribute ID
     *  \return options
     */
    std::string getEdgeAttributeOptions(const libgexf::t_id attr_id) const;

    /*!
     *  \brief Get the options of a node attribute
     *
     *  \param attr_id : attribute ID
     *  \return options
     */
    std::string getNodeAttributeOptions(const libgexf::t_id attr_id) const;

    /*!
     *  \brief Get the default value of an edge attribute
     *
     *  \param attr_id : attribute ID
     *  \return default value
     */
    std::string getEdgeAttributeDefault(const libgexf::t_id attr_id) const;

    
    /*!
     *  \brief Check if a node attribute has a default value
     *
     *  \param attr_id : attribute ID
     *  \return true if the default value exists, false otherwise
     */
    bool hasNodeAttributeDefault(const libgexf::t_id attr_id) const;

    /*!
     *  \brief Check if an edge attribute has a default value
     *
     *  \param attr_id : attribute ID
     *  \return true if the default value exists, false otherwise
     */
    bool hasEdgeAttributeDefault(const libgexf::t_id attr_id) const;


    /*!
     *  \brief Check if a node attribute has options
     *
     *  \param attr_id : attribute ID
     *  \return true if options exists, false otherwise
     */
    bool hasNodeAttributeOptions(const libgexf::t_id attr_id) const;

    /*!
     *  \brief Check if an edge attribute has options
     *
     *  \param attr_id : attribute ID
     *  \return true if options exists, false otherwise
     */
    bool hasEdgeAttributeOptions(const libgexf::t_id attr_id) const;


    /*!
     *  \brief Check if an option is available for the given node attribute
     *
     *  \param attr_id : attribute ID
     *  \param option : option strings separated by pipes (|)
     *  \return true if the option is available, false otherwise
     */
    bool isNodeAttributeOption(const libgexf::t_id attr_id, const std::string& option) const;

    /*!
     *  \brief Check if an option is available for the given edge attribute
     *
     *  \param attr_id : attribute ID
     *  \param option : option strings separated by pipes (|)
     *  \return true if the option is available, false otherwise
     */
    bool isEdgeAttributeOption(const libgexf::t_id attr_id, const std::string& option) const;


    // TODO
    void removeNodeAttributeColumn(const libgexf::t_id attr_id);

    // TODO
    void removeEdgeAttributeColumn(const libgexf::t_id attr_id);

    /*!
     *  \brief Delete all attribute values for a node
     *
     *  \param node_id : node ID
     */
    void clearNodeAttributes(const libgexf::t_id node_id);

    /*!
     *  \brief Delete all attribute values for an edge
     *
     *  \param edge_id : edge ID
     */
    void clearEdgeAttributes(const libgexf::t_id edge_id);

    /*!
     *  \brief Clear all attributes (columns an rows)
     *
     */
    void clear();

    /*!
     *  \brief Clear edge attributes (columns an rows), and labels
     *
     */
    void clearEdgesAttributes();
private:
    std::map<t_id,std::string > _node_labels; /*!< map<node_id, label > */
    std::map<t_id,std::string > _edge_labels; /*!< map<edge_id, label > */
    
    std::map<t_id,std::string > _node_attributes; /*!< map<attr_id, title > */
    std::map<t_id,t_attr_type > _node_attributes_types; /*!< map<attr_id, type > */
    std::map<t_id,std::string > _node_default_values; /*!< map<attr_id, default > */
    std::map<t_id,std::map<std::string,unsigned short int > > _node_options; /*!< map<attr_id, option[] > */
    std::map<t_id,std::string > _node_verbatim_options; /*!< gexf strings */
    std::map<t_id,std::map<t_id,std::string > > _node_values; /*!< map<node_id, map<attr_id, value > > */

    std::map<t_id,std::string > _edge_attributes; /*!< map<attr_id, title > */
    std::map<t_id,t_attr_type > _edge_attributes_types; /*!< map<attr_id, type > */
    std::map<t_id,std::string > _edge_default_values; /*!< map<attr_id, default > */
    std::map<t_id,std::map<std::string,unsigned short int > > _edge_options; /*!< map<attr_id, option[] > */
    std::map<t_id,std::string > _edge_verbatim_options; /*!< gexf strings */
    std::map<t_id,std::map<t_id,std::string > > _edge_values; /*!< map<edge_id, map<attr_id, value > > */
private:
    void init();
    friend std::ostream& operator<<(std::ostream& os, const Data& o);
    unsigned int countNodeAttributeColumn() const;
    unsigned int countEdgeAttributeColumn() const;
    friend class AttributeIter; /*!< AttributeIter */
    friend class AttValueIter; /*!< AttValueIter */
};

} /* namespace libgexf */

#endif	/* _DATA_H */

