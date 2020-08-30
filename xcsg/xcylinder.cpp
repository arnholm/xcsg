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

#include "xcylinder.h"
#include "primitives3d.h"
#include "csg_parser/cf_xmlNode.h"
#include "carve/mesh_simplify.hpp"

static const double pi = 4.0*atan(1.0);

xcylinder::xcylinder(double h, double r, bool center )
: m_h(h)
, m_r(r)
, m_center(center)
{}

xcylinder::~xcylinder()
{}

std::shared_ptr<carve::mesh::MeshSet<3>> xcylinder::create_carve_mesh(const carve::math::Matrix& t) const
{
   const int nseg = -1;
   std::shared_ptr<xpolyhedron> poly = primitives3d::make_cone(m_r,m_r,m_h,m_center,nseg,t*get_transform());
   std::shared_ptr<carve::mesh::MeshSet<3>> meshset = poly->create_carve_mesh();

   carve::mesh::MeshSimplifier simplifier;
   double min_normal_angle=(pi/180.)*1E-4;  // 1E-4 degrees
   simplifier.mergeCoplanarFaces(meshset.get(),min_normal_angle);

   return meshset;
}

xcylinder::xcylinder(const cf_xmlNode& node)
{
   if(node.tag() != "cylinder")throw logic_error("Expected xml tag cylinder, but found " + node.tag());
   if(!node.has_property("h"))throw logic_error("Required attribute 'h' not found for " + node.tag());
   if(!node.has_property("r"))throw logic_error("Required attribute 'r' not found for " + node.tag());

   set_transform(node);
   m_h   = node.get_property("h",1.0);
   m_r   = node.get_property("r",1.0);
   m_center = ("true" == node.get_property("center","false"))? true : false;
}
