// BeginLicense:
// Part of: xcsg - XML based Constructive Solid Geometry
// Copyright (C) 2017-2020 Carsten Arnholm
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

#ifndef CF_XMLTREE_H
#define CF_XMLTREE_H

// Parsing xml with Boost
// http://akrzemi1.wordpress.com/2011/07/13/parsing-xml-with-boost/

#include "cf_xmlNode.h"

#include <ostream>
#include <string>
using namespace std;
#include <boost/property_tree/ptree.hpp>

class cf_xmlTree {
public:
   typedef boost::property_tree::ptree ptree;

   cf_xmlTree();
   virtual ~cf_xmlTree();

   // clear the tree completely
   void clear();

   // create the root node of the tree, return false if it already exists
   // use get_root to access it later
   bool create_root(const string& root_name);

   // return the root node of the tree, if it exists
   bool get_root(cf_xmlNode& root);

   // write xml data to any output stream
   bool write_xml(ostream& os);

   // write xml data to file
   bool write_xml(const string& path);

   // read xml data from any input stream
   bool read_xml(istream& is);

   // read xml data from file
   bool read_xml(const string& path);

   // === JSON export/import

   // write json data to any output stream, set pretty=false to make a compact file
   bool write_json(ostream& os, bool pretty);

   // write json data to file, set pretty=false to make a compact file
   bool write_json(const string& path, bool pretty = true);

   // read json data from any input stream
   bool read_json(istream& is);

   // read json data from any file
   bool read_json(const string& path);

private:
   string  m_root_name;
   ptree   m_tree;
};

#endif // cf_xmlTree_H
