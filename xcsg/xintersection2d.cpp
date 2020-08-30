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

#include "xintersection2d.h"
#include "clipper_boolean.h"
#include "carve_boolean.h"
#include "csg_parser/cf_xmlNode.h"
#include "xcsg_factory.h"
#include "xshape2d_collector.h"

xintersection2d::xintersection2d()
{}

xintersection2d::~xintersection2d()
{}

size_t xintersection2d::nbool()
{
   size_t nbool = 0;
   for(auto i=m_incl.begin(); i!=m_incl.end(); i++) {
      std::shared_ptr<xshape2d> obj = *i;
      nbool += (obj->nbool()+1);
   }
   return nbool-1;
}

std::shared_ptr<clipper_profile> xintersection2d::create_clipper_profile(const carve::math::Matrix& t) const
{
   clipper_boolean csg;
   size_t icount=0;
   for(auto i=m_incl.begin(); i!=m_incl.end(); i++) {
      if(icount == 0) csg.compute((*i)->create_clipper_profile(t*get_transform()),ClipperLib::ctUnion);
      else            csg.compute((*i)->create_clipper_profile(t*get_transform()),ClipperLib::ctIntersection);
      icount++;
   }
   return csg.profile();
}

std::shared_ptr<carve::mesh::MeshSet<3>> xintersection2d::create_carve_mesh(const carve::math::Matrix& t) const
{
   carve_boolean csg;

   size_t icount=0;
   for(auto i=m_incl.begin(); i!=m_incl.end(); i++) {
      std::shared_ptr<xshape2d> obj = *i;
      if(icount == 0) csg.compute(obj->create_carve_mesh(t*get_transform()),carve::csg::CSG::UNION);
      else            csg.compute(obj->create_carve_mesh(t*get_transform()),carve::csg::CSG::INTERSECTION);
      icount++;
   }
   csg.eliminate_short_edges();
   return csg.mesh_set();
}

xintersection2d::xintersection2d(const cf_xmlNode& node)
{
   if(node.tag() != "intersection2d")throw logic_error("Expected xml tag intersection2d, but found " + node.tag());
   set_transform(node);

   xshape2d_collector::collect_children(node,m_incl);
}
