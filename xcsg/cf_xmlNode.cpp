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
   
#include "cf_xmlNode.h"

#include <boost/algorithm/string.hpp>
using namespace std;
using namespace boost::algorithm;

cf_xmlNode::cf_xmlNode()
{}

cf_xmlNode::cf_xmlNode(const string& tag, ptree& ptree_node)
: m_tag(tag)
, m_ptree_node(ptree_node)
{}

cf_xmlNode::cf_xmlNode(iterator it)
: m_tag(it->first)
, m_ptree_node(it->second)
{}

cf_xmlNode& cf_xmlNode::operator=(const cf_xmlNode& other_node )
{
   m_tag        = other_node.m_tag;
   m_ptree_node = other_node.m_ptree_node;
   return *this;
}

cf_xmlNode::~cf_xmlNode()
{}

bool cf_xmlNode::is_valid() const
{
   return (bool)m_ptree_node;
}

string cf_xmlNode::tag() const
{
   return m_tag;
}

bool cf_xmlNode::is_attribute_node() const
{
   return ("<xmlattr>" == m_tag);
}

bool cf_xmlNode::add_property(const string& name, const string& value)
{
   if(m_ptree_node) {
      string attrib = "<xmlattr>." + name;
      m_ptree_node.get().put(attrib,value);
      return true;
   }
   return false;
}

bool cf_xmlNode::add_property(const string& name, size_t value)
{
   if(m_ptree_node) {
      string attrib = "<xmlattr>." + name;
      m_ptree_node.get().put(attrib,value);
      return true;
   }
   return false;
}

bool cf_xmlNode::add_property(const string& name, int value)
{
   if(m_ptree_node) {
      string attrib = "<xmlattr>." + name;
      m_ptree_node.get().put(attrib,value);
      return true;
   }
   return false;
}

bool cf_xmlNode::add_property(const string& name, double value)
{
   if(m_ptree_node) {
      string attrib = "<xmlattr>." + name;
      m_ptree_node.get().put(attrib,value);
      return true;
   }
   return false;
}

bool cf_xmlNode::has_property(const string& name) const
{
   if(m_ptree_node) {
      string attrib = "<xmlattr>." + name;
      boost::optional<string> v = m_ptree_node.get().get_optional<string>(attrib); // anything can be represented as a string
      return (bool)v;
   }
   return false;
}

string cf_xmlNode::get_property(const string& name, const string& default_value) const
{
   if(m_ptree_node) {
      string attrib = "<xmlattr>." + name;
      boost::optional<string> v = m_ptree_node.get().get_optional<string>(attrib);
      if(v) {
         trim(v.get());
         return v.get();
      }
   }
   return  default_value;
}

size_t cf_xmlNode::get_property(const string& name, size_t default_value) const
{
   if(m_ptree_node) {
      string attrib = "<xmlattr>." + name;
      boost::optional<size_t> v = m_ptree_node.get().get_optional<size_t>(attrib);
      if(v) return v.get();
   }
   return  default_value;
}


int    cf_xmlNode::get_property(const string& name, int default_value) const
{
   if(m_ptree_node) {
      string attrib = "<xmlattr>." + name;
      boost::optional<int> v = m_ptree_node.get().get_optional<int>(attrib);
      if(v) return v.get();
   }
   return  default_value;
}

double cf_xmlNode::get_property(const string& name, double default_value) const
{
   if(m_ptree_node) {
      string attrib = "<xmlattr>." + name;
      boost::optional<double> v = m_ptree_node.get().get_optional<double>(attrib);
      if(v) return v.get();
      return  default_value;
   }
   return  default_value;
}

bool cf_xmlNode::put_value(const string& value)
{
   if(m_ptree_node) {
      m_ptree_node.get().put_value(value);
      return true;
   }
   return false;
}

bool cf_xmlNode::put_value(size_t value)
{
   if(m_ptree_node) {
      m_ptree_node.get().put_value(value);
      return true;
   }
   return false;
}

bool cf_xmlNode::put_value(int value)
{
   if(m_ptree_node) {
      m_ptree_node.get().put_value(value);
      return true;
   }
   return false;
}

bool cf_xmlNode::put_value(double value)
{
   if(m_ptree_node) {
      m_ptree_node.get().put_value(value);
      return true;
   }
   return false;
}

string cf_xmlNode::get_value(const string& default_value) const
{
   if(m_ptree_node) {
      boost::optional<string> v = m_ptree_node.get().get_value_optional<string>();
      if(v) return v.get();
      return  default_value;
   }
   return  default_value;
}

size_t cf_xmlNode::get_value(size_t default_value) const
{
   if(m_ptree_node) {
      boost::optional<size_t> v = m_ptree_node.get().get_value_optional<size_t>();
      if(v) return v.get();
      return  default_value;
   }
   return  default_value;
}

int  cf_xmlNode::get_value(int default_value) const
{
   if(m_ptree_node) {
      boost::optional<int> v = m_ptree_node.get().get_value_optional<int>();
      if(v) return v.get();
      return  default_value;
   }
   return  default_value;
}

double cf_xmlNode::get_value(double default_value) const
{
   if(m_ptree_node) {
      boost::optional<double> v = m_ptree_node.get().get_value_optional<double>();
      if(v) return v.get();
      return  default_value;
   }
   return  default_value;
}

bool cf_xmlNode::has_value() const
{
   if(m_ptree_node) {
      boost::optional<string> v = m_ptree_node.get().get_value_optional<string>(); // anything can be represented as a string
      return (bool)v;
   }
   return false;
}

cf_xmlNode cf_xmlNode::add_child(const string& tag)
{
   if(m_ptree_node) {
      return cf_xmlNode(tag, m_ptree_node.get().add_child(tag,ptree()));
   }
   return cf_xmlNode();
}

bool cf_xmlNode::get_child(const string& tag, cf_xmlNode& child) const
{
   if(m_ptree_node) {
      boost::optional<ptree &> child_node = m_ptree_node.get().get_child_optional(tag);
      if(child_node) {
         child = cf_xmlNode(tag, child_node.get());
         return true;
      }
   }
   return  false;
}

bool cf_xmlNode::get_property_node(cf_xmlNode& pnode)
{
   return get_child("<xmlattr>",pnode);
}

cf_xmlNode::iterator cf_xmlNode::begin()
{
   return m_ptree_node.get().begin();
}

cf_xmlNode::iterator cf_xmlNode::end()
{
   return m_ptree_node.get().end();
}

cf_xmlNode::const_iterator cf_xmlNode::begin() const
{
   return m_ptree_node.get().begin();
}

cf_xmlNode::const_iterator cf_xmlNode::end() const
{
   return m_ptree_node.get().end();
}

// erase the child pointed at by the iterator. "it_child" becomes invalid.
// returns a valid iterator pointing to the element after the erased.
cf_xmlNode::iterator cf_xmlNode::erase(iterator it_child)
{
   return m_ptree_node.get().erase(it_child);
}
