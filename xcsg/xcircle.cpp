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

#include "xcircle.h"
#include "primitives2d.h"
#include "primitives3d.h"
#include "csg_parser/cf_xmlNode.h"

xcircle::xcircle(double r)
: m_r(r)
{}

xcircle::xcircle(const cf_xmlNode& node)
{
   if(node.tag() != "circle")throw logic_error("Expected xml tag 'circle', but found " + node.tag());
   set_transform(node);

   if(!node.has_property("r"))throw logic_error("Required attribute 'r' not found for " + node.tag());

   m_r  = node.get_property("r",1.0);
}

xcircle::~xcircle()
{}

std::shared_ptr<clipper_profile> xcircle::create_clipper_profile(const carve::math::Matrix& t) const
{
   int  nseg = -1;
   std::shared_ptr<polygon2d> poly = primitives2d::make_circle(m_r,nseg,t*get_transform());
   std::shared_ptr<clipper_profile> mesh(new clipper_profile());
   mesh->AddPaths(poly->paths());
   return mesh;
}

std::shared_ptr<carve::mesh::MeshSet<3>> xcircle::create_carve_mesh(const carve::math::Matrix& t) const
{
   int  nseg = -1;
   std::shared_ptr<xpolyhedron> poly = primitives3d::make_cone(m_r,m_r, mesh_utils::thickness(),false,nseg,t*get_transform());
   return poly->create_carve_mesh();
}
