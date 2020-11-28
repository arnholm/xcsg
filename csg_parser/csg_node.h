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

#ifndef CSG_NODE_H
#define CSG_NODE_H

#include <string>
#include <memory>
#include <vector>
#include <map>

#include "csg_value.h"
#include "cf_xmlNode.h"
#include "csg_matrix.h"

// csg_node is a node in the openscad tree
class csg_node {
   friend class csg_parser;
public:
   typedef std::map<std::string,std::string> xmap;
   static void configure_xmap();

   typedef std::pair<std::string,std::pair<size_t,size_t>> func_data;  // <signature,<tree_level,line>>

   // for parameter list
   typedef std::map<std::string,std::shared_ptr<csg_value>> par_map;
   typedef par_map::iterator par_iterator;

   csg_node();
   csg_node(size_t level, size_t line_no, const std::string& func);
   virtual ~csg_node();

   // return naked function name
   std::string tag() const;

   // return full function string
   std::string func() const { return m_func; }

   size_t line_no() const { return m_line_no; }

   // look up parameter
   par_iterator par_find(const std::string& name) { return m_par.find(name); };
   par_iterator par_begin() { return m_par.begin(); }
   par_iterator par_end()   { return m_par.end(); }

   // helpers for parameters
   std::string  get_scalar(const std::string& name);
   std::shared_ptr<csg_value> get_value(const std::string& name);

   cf_xmlNode to_xcsg(cf_xmlNode& parent);

   // returns 0 for no children
   // returns 2 for 2d children
   // returns 3 for 3d children
   size_t dimension();

   // count non-dummy children
   size_t size_children();

protected:

   // helpers to buuild csg tree, parse node parameters etc
   void build_tree(const std::vector<func_data>& func, size_t& index);
   void parse_params();
   std::string par_value(std::string& param, size_t istart);

   void dump();

   // dummy nodes such as group(); with zero children
   bool is_dummy();

   // return parameter substring
   std::string par() const;

   // parameter name from position
   static std::string par_name(size_t ipos);

   // add child node to this node
   void push_back(std::shared_ptr<csg_node> child);

   // assign node transformation from multmatrix
   void assign_matrix();

   // export matrix to xcsg for given target node
   void to_xcsg(cf_xmlNode& target, csg_matrix<4,4>& matrix);

   // replace * suffix with proper 2d or 3d suffix
   std::string fix_tag(const std::string& tag);

private:
   int                                    m_level;     // level of this node in tree (root has level -1)
   size_t                                 m_line_no;   // csg file line no
   std::string                            m_func;      // openscad function signature
   par_map                                m_par;       // parsed parameter list
   std::vector<std::shared_ptr<csg_node>> m_children;  // child nodes

   bool                                   m_has_matrix;
   csg_matrix<4,4>                        m_matrix;    // assigned only for multmatrix nodes

   static xmap  m_xmap;
};

#endif // CSG_NODE_H
