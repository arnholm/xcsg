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
   
#include "xoffset2d.h"
#include "cf_xmlNode.h"
#include "xcsg_factory.h"
#include "clipper_boolean.h"
#include "clipper_csg/clipper_offset.h"
#include "xshape2d_collector.h"

xoffset2d::xoffset2d()
{}

xoffset2d::~xoffset2d()
{}

xoffset2d::xoffset2d(const cf_xmlNode& node)
: m_delta(0.0)
, m_round(true)
, m_chamfer(0.0)
{
   if(node.tag() != "offset2d")throw logic_error("Expected xml tag offset2d, but found " + node.tag());
   set_transform(node);

   string default_round   = ((m_round)?   "true" : "false");
   string default_chamfer = ((m_chamfer)? "true" : "false");

   m_delta   = node.get_property("delta",m_delta);
   m_round   = ((node.get_property("round",default_round)    =="true")? true : false);
   m_chamfer = ((node.get_property("chamfer",default_chamfer)=="true")? true : false);

   xshape2d_collector::collect_children(node,m_incl);
}


size_t xoffset2d::nbool()
{
   size_t nbool = 0;
   for(auto i=m_incl.begin(); i!=m_incl.end(); i++) {
      std::shared_ptr<xshape2d> obj = *i;
      nbool += (obj->nbool()+1);
   }
   return nbool-1;
}

std::shared_ptr<clipper_profile> xoffset2d::create_clipper_profile(const carve::math::Matrix& t) const
{
   // first union together the components (usually only one)
   clipper_boolean csg;
   for(auto i=m_incl.begin(); i!=m_incl.end(); i++) {
      csg.compute((*i)->create_clipper_profile(t*get_transform()),ClipperLib::ctUnion);
   }

   // then compute offset profile and return it

   clipper_offset offset;
   offset.compute(csg.profile(),m_delta,m_round,m_chamfer);
   return offset.profile();
}

std::shared_ptr<carve::mesh::MeshSet<3>> xoffset2d::create_carve_mesh(const carve::math::Matrix& t) const
{
   return 0;
}
