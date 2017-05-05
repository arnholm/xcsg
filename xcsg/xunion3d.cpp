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
   
#include "xunion3d.h"
#include "carve_boolean.h"
#include "cf_xmlNode.h"
#include "xcsg_factory.h"
#include "xsolid_collector.h"

xunion3d::xunion3d()
{}

xunion3d::xunion3d(const cf_xmlNode& node)
{
   if(node.tag() != "union3d")throw logic_error("Expected xml tag union3d, but found " + node.tag());
   set_transform(node);
   xsolid_collector::collect_children(node,m_incl);
}

xunion3d::~xunion3d()
{}

size_t xunion3d::nbool()
{
   size_t nbool = 0;
   for(auto i=m_incl.begin(); i!=m_incl.end(); i++) {
      std::shared_ptr<xsolid> obj = *i;
      nbool += (obj->nbool()+1);
   }
   return nbool-1;
}

std::shared_ptr<carve::mesh::MeshSet<3>> xunion3d::create_carve_mesh(const carve::math::Matrix& t) const
{
   carve_boolean csg;
   for(auto i=m_incl.begin(); i!=m_incl.end(); i++) {
      csg.compute((*i)->create_carve_mesh(t*get_transform()),carve::csg::CSG::UNION);
   }
   return csg.mesh_set();
}
