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

#include "csg_parser.h"
#include <sstream>
#include <vector>
#include <iomanip>

csg_parser::csg_parser(std::istream& csg)
: m_root(std::make_shared<csg_node>())
{
   // read the whole file into a string
   std::ostringstream sstr;
   sstr << csg.rdbuf();
   init_func(sstr.str());
   // m_root->dump();
}

csg_parser::~csg_parser()
{}

void csg_parser::init_func(const std::string& csg)
{
   std::vector<func_data>   func;  // function calls, by function index

   std::string token;

   // level in tree
   size_t level = 0;
   size_t line_no = 1;

   size_t i=0;
   while(i<csg.size()) {

      // get current character
      char c = csg[i++];

      // Skip // comment line
      while(c=='/' && csg[i]=='/') {
         while(csg[++i]!='\n') c = csg[i];
         c = csg[i];
      }

      // update the tree level
           if( c=='{') ++level;
      else if( c=='}') --level;

      if( c=='\n') {
         line_no++;
         token.clear();
      }

      // characters not contributing to token
      if( c==' ' || c=='\t' || c=='\n' || c==';' || c=='{' || c=='}' || c=='#' || c=='%') continue;

      // add character to token
      token += c;

      if(c==')') {
         // end of token
         func.push_back(func_data(token,std::make_pair(level,line_no)));
         token.clear();
      }

   }

   // build the tree
   size_t index = 0;
   m_root->build_tree(func,index);
//   m_root->dump();
}

bool csg_parser::to_xcsg(cf_xmlTree& tree)
{
   csg_node::configure_xmap();

   if(tree.create_root("xcsg")) {
      cf_xmlNode root;
      if(tree.get_root(root)) {
         root.add_property("version","1.0");
         root.add_property("secant_tolerance","0.05");

         m_root->to_xcsg(root);
         if(m_root->dimension() < 2) throw std::runtime_error("Undetermined .csg model dimension, conversion failed.");

         return true;
      }
   }
   return false;
}
