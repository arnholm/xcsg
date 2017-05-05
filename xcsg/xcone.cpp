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

#include "xcone.h"
#include "primitives3d.h"
#include "cf_xmlNode.h"

xcone::xcone(double h, double r1, double r2, bool center)
: m_h(h)
, m_r1(r1)
, m_r2(r2)
, m_center(center)
{}

xcone::xcone(const cf_xmlNode& node)
{
   if(node.tag() != "cone")throw logic_error("Expected xml tag 'cone', but found " + node.tag());

   set_transform(node);
   if(!node.has_property("h"))throw logic_error("Required attribute 'h' not found for " + node.tag());
   if(!node.has_property("r1"))throw logic_error("Required attribute 'r1' not found for " + node.tag());
   if(!node.has_property("r2"))throw logic_error("Required attribute 'r2' not found for " + node.tag());

   m_h  = node.get_property("h",1.0);
   m_r1 = node.get_property("r1",1.0);
   m_r2 = node.get_property("r2",1.0);
   m_center = ("true" == node.get_property("center","false"))? true : false;
}

xcone::~xcone()
{}

std::shared_ptr<carve::mesh::MeshSet<3>> xcone::create_carve_mesh(const carve::math::Matrix& t) const
{
   int nseg = -1;
   std::shared_ptr<xpolyhedron> poly = primitives3d::make_cone(m_r1,m_r2,m_h,m_center,nseg,t*get_transform());

   std::shared_ptr<carve::mesh::MeshSet<3>> mesh = poly->create_carve_mesh();
   return mesh;
}
