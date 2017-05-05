// BeginLicense:
// Part of: xcsg - XML based Constructive Solid Geometry
// Copyright (C) 2017 Carsten Arnholm
// All rights reserved
//
// This file may be used under the terms of either the GNU General
// Public License version 2 or 3 (at your option) as published by the
// Free Software Foundation and appearing in the files LICENSE.GPL2
// and LICENSE.GPL3 included in the packaging of this file.
// 
// This file is provided "AS IS" with NO WARRANTY OF ANY KIND,
// INCLUDING THE WARRANTIES OF DESIGN, MERCHANTABILITY AND FITNESS FOR
// A PARTICULAR PURPOSE. ALL COPIES OF THIS FILE MUST INCLUDE THIS LICENSE.
// EndLicense:
   
#ifndef CF_XMLNODE_H
#define CF_XMLNODE_H

#include <boost/property_tree/ptree.hpp>
#include <ostream>
#include <string>
using namespace std;

class cf_xmlNode {
   friend class cf_xmlTree;
public:

   typedef boost::property_tree::ptree ptree;
   typedef ptree::iterator iterator;
   typedef ptree::const_iterator const_iterator;

   cf_xmlNode();
   cf_xmlNode& operator=(const cf_xmlNode& other_node );
   virtual ~cf_xmlNode();

   // if the object actually refers to a position in the tree, the node is valid
   bool is_valid() const;

   // return the tag of the node.
   // NOTE: The tag "<xmlattr>" is reserved and indicates a node containing attributes.
   // Du NOT try to read properties from such a node. Use instead get_property(...) for the the parent node.
   string tag() const;

   // return true if this is a special node containing attributes (ref.  "<xmlattr>" )
   bool is_attribute_node() const;

   // ====== properties

   // add properties to this node
   bool add_property(const string& name, const string& value);
   bool add_property(const string& name, size_t value);
   bool add_property(const string& name, int value);
   bool add_property(const string& name, double value);

   // check if named property exists
   bool   has_property(const string& name) const;

   // return properties of this node, return default value if not found
   string get_property(const string& name, const string& default_value) const;
   size_t get_property(const string& name, size_t default_value) const;
   int    get_property(const string& name, int default_value) const;
   double get_property(const string& name, double default_value) const;

   // store a value directly in this node <tag> value </tag>
   bool put_value(const string& value);
   bool put_value(size_t value);
   bool put_value(int value);
   bool put_value(double value);

   // return a value directly in this node <tag> value </tag> , return default value if not found
   string get_value(const string& default_value) const;
   size_t get_value(size_t default_value) const;
   int    get_value(int default_value) const;
   double get_value(double default_value) const;

   // check if a value exists on this node
   bool has_value() const;

   // ====== child nodes

   // add a child node to this node, return a child reference
   cf_xmlNode add_child(const string& tag);

   // return the (first) named child if such exists
   bool get_child(const string& tag, cf_xmlNode& child) const;

   // return the property child node to allow property traversal. Use with care
   bool get_property_node(cf_xmlNode& pnode);

   // traverse all the immediate children of the node
   iterator begin();
   iterator end();

   const_iterator begin() const;
   const_iterator end() const;

   // convert iterator to node
   cf_xmlNode(iterator it);

   // erase the child pointed at by the iterator. "it_child" becomes invalid.
   // returns a valid iterator pointing to the element after the erased.
   iterator erase(iterator it_child);

private:
   cf_xmlNode(const string& tag, ptree& m_ptree_node);

private:
   string  m_tag;
   boost::optional<ptree &> m_ptree_node;
};

#endif // CF_XMLNODE_H
