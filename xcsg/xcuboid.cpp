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

#include "xcuboid.h"
#include "primitives3d.h"
#include "cf_xmlNode.h"

xcuboid::xcuboid(double dx, double dy, double dz, bool center)
: m_dx(dx)
, m_dy(dy)
, m_dz(dz)
, m_center(center)
{}

xcuboid::~xcuboid()
{}


std::shared_ptr<carve::mesh::MeshSet<3>> xcuboid::create_carve_mesh(const carve::math::Matrix& t) const
{
   std::shared_ptr<xpolyhedron> poly = primitives3d::make_cuboid(m_dx,m_dy,m_dz,m_center,m_center,t*get_transform());
   return poly->create_carve_mesh();
}


xcuboid::xcuboid(const cf_xmlNode& node)
{
   if(node.tag() != "cuboid")throw logic_error("Expected xml tag cuboid, but found " + node.tag());
   if(!node.has_property("dx"))throw logic_error("Required attribute 'dx' not found for " + node.tag());
   if(!node.has_property("dy"))throw logic_error("Required attribute 'dy' not found for " + node.tag());
   if(!node.has_property("dz"))throw logic_error("Required attribute 'dz' not found for " + node.tag());


   set_transform(node);
   m_dx  = node.get_property("dx",1.0);
   m_dy  = node.get_property("dy",1.0);
   m_dz  = node.get_property("dz",1.0);
   m_center = ("true" == node.get_property("center","false"))? true : false;
}
