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

#include "xcube.h"
#include "primitives3d.h"
#include "cf_xmlNode.h"

xcube::xcube(double size, bool center)
: m_size(size)
, m_center(center)
{}

xcube::~xcube()
{}

std::shared_ptr<carve::mesh::MeshSet<3>> xcube::create_carve_mesh(const carve::math::Matrix& t) const
{
   std::shared_ptr<xpolyhedron> poly = primitives3d::make_cuboid(m_size,m_size,m_size,m_center,m_center,t*get_transform());
   return poly->create_carve_mesh();
}

xcube::xcube(const cf_xmlNode& node)
{
   if(node.tag() != "cube")throw logic_error("Expected xml tag cube, but found " + node.tag());
   if(!node.has_property("size"))throw logic_error("Required attribute 'size' not found for " + node.tag());

   set_transform(node);
   m_size   = node.get_property("size",1.0);
   m_center = ("true" == node.get_property("center","false"))? true : false;
}
