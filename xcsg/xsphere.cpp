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

#include "xsphere.h"
#include "primitives3d.h"

xsphere::xsphere(double r)
: m_r(r)
{}

xsphere::xsphere(const cf_xmlNode& node)
: m_r(0.0)
{
   if(node.tag() != "sphere")throw logic_error("Expected xml tag sphere, but found " + node.tag());
   if(!node.has_property("r"))throw logic_error("Required attribute 'r' not found for " + node.tag());

   set_transform(node);
   m_r  = node.get_property("r",1.0);
}

xsphere::~xsphere()
{}

std::shared_ptr<carve::mesh::MeshSet<3>> xsphere::create_carve_mesh(const carve::math::Matrix& t) const
{
   int nseg = -1;
 //  std::shared_ptr<xpolyhedron> poly = primitives3d::make_sphere(m_r,nseg,t*get_transform());
   std::shared_ptr<xpolyhedron> poly = primitives3d::make_geodesic_sphere(m_r,nseg,t*get_transform());
   return poly->create_carve_mesh();
}
