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
   
#include "xdifference3d.h"
#include "carve_boolean.h"
#include "cf_xmlNode.h"
#include "xcsg_factory.h"
#include "xsolid_collector.h"

xdifference3d::xdifference3d( )
{}

xdifference3d::~xdifference3d()
{}


std::shared_ptr<carve::mesh::MeshSet<3>> xdifference3d::create_carve_mesh(const carve::math::Matrix& t) const
{
   carve_boolean a,b;
   for(auto i=m_incl.begin(); i!=m_incl.end(); i++) {
      std::shared_ptr<xsolid> obj = *i;
      a.compute(obj->create_carve_mesh(t*get_transform()),carve::csg::CSG::UNION);
   }

   for(auto i=m_excl.begin(); i!=m_excl.end(); i++) {
      std::shared_ptr<xsolid> obj = *i;
      b.compute(obj->create_carve_mesh(t*get_transform()),carve::csg::CSG::UNION);
   }

   carve_boolean csg;
   csg.compute(a.mesh_set(),carve::csg::CSG::UNION);
   csg.compute(b.mesh_set(),carve::csg::CSG::A_MINUS_B);
   csg.eliminate_short_edges();

   return csg.mesh_set();
}


size_t xdifference3d::nbool()
{
   size_t nbool = 0;
   for(auto i=m_incl.begin(); i!=m_incl.end(); i++) {
      std::shared_ptr<xsolid> obj = *i;
      nbool += (obj->nbool()+1);
   }
   for(auto i=m_excl.begin(); i!=m_excl.end(); i++) {
      std::shared_ptr<xsolid> obj = *i;
      nbool += (obj->nbool()+1);
   }
   return nbool-1;
}

xdifference3d::xdifference3d(const cf_xmlNode& node)
{
   if(node.tag() != "difference3d")throw logic_error("Expected xml tag difference3d, but found " + node.tag());
   set_transform(node);

   xsolid_collector::collect_children(node,m_incl,1,m_excl);
}
