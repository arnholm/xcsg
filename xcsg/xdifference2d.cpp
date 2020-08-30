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

#include "xdifference2d.h"
#include "clipper_boolean.h"
#include "carve_boolean.h"
#include "csg_parser/cf_xmlNode.h"
#include "xcsg_factory.h"
#include "xshape2d_collector.h"

xdifference2d::xdifference2d( )
{}

xdifference2d::~xdifference2d()
{}

std::shared_ptr<clipper_profile> xdifference2d::create_clipper_profile(const carve::math::Matrix& t) const
{
   clipper_boolean a,b;
   for(auto i=m_incl.begin(); i!=m_incl.end(); i++) {
      a.compute((*i)->create_clipper_profile(t*get_transform()),ClipperLib::ctUnion);
   }

   for(auto i=m_excl.begin(); i!=m_excl.end(); i++) {
      b.compute((*i)->create_clipper_profile(t*get_transform()),ClipperLib::ctUnion);
   }

   clipper_boolean csg;
   csg.compute(a.profile(),ClipperLib::ctUnion);
   csg.compute(b.profile(),ClipperLib::ctDifference);

   return csg.profile();
}



std::shared_ptr<carve::mesh::MeshSet<3>> xdifference2d::create_carve_mesh(const carve::math::Matrix& t) const
{
   carve_boolean a,b;
   for(auto i=m_incl.begin(); i!=m_incl.end(); i++) {
      std::shared_ptr<xshape2d> obj = *i;
      a.compute(obj->create_carve_mesh(t*get_transform()),carve::csg::CSG::UNION);
   }

   for(auto i=m_excl.begin(); i!=m_excl.end(); i++) {
      std::shared_ptr<xshape2d> obj = *i;
      b.compute(obj->create_carve_mesh(t*get_transform()),carve::csg::CSG::UNION);
   }

   carve_boolean csg;
   csg.compute(a.mesh_set(),carve::csg::CSG::UNION);
   csg.compute(b.mesh_set(),carve::csg::CSG::A_MINUS_B);
   return csg.mesh_set();
}

size_t xdifference2d::nbool()
{
   size_t nbool = 0;
   for(auto i=m_incl.begin(); i!=m_incl.end(); i++) {
      std::shared_ptr<xshape2d> obj = *i;
      nbool += (obj->nbool()+1);
   }
   for(auto i=m_excl.begin(); i!=m_excl.end(); i++) {
      std::shared_ptr<xshape2d> obj = *i;
      nbool += (obj->nbool()+1);
   }
   return nbool-1;
}

xdifference2d::xdifference2d(const cf_xmlNode& node)
{
   if(node.tag() != "difference2d")throw logic_error("Expected xml tag difference2d, but found " + node.tag());
   set_transform(node);

   xshape2d_collector::collect_children(node,m_incl,1,m_excl);
}
