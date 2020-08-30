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

#ifndef CSG_PARSER_H
#define CSG_PARSER_H

#include <iostream>
#include <string>
#include <vector>
#include "cf_xmlTree.h"

#include "csg_node.h"

// csg_parser reads an openscad .csg file, builds a tree amd exports to xcsg
class csg_parser {
public:
   typedef std::pair<std::string,std::pair<size_t,size_t>> func_data;  // <signature,<tree_level,line>>

   // parse openscad.csg file and build the openscad tree
   csg_parser(std::istream& csg);
   virtual ~csg_parser();

   // create an xcsg tree
   bool to_xcsg(cf_xmlTree& tree);

protected:
   void init_func(const std::string& csg);

private:
   std::shared_ptr<csg_node>  m_root;  // openscad tree
};

#endif // CSG_PARSER_H
