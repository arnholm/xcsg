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

#include "xsquare.h"
#include "primitives2d.h"
#include "primitives3d.h"
#include "csg_parser/cf_xmlNode.h"


xsquare::xsquare(double size, bool center)
: m_size(size)
, m_center(center)
{}

xsquare::xsquare(const cf_xmlNode& node)
{
   if(node.tag() != "square")throw logic_error("Expected xml tag 'square', but found " + node.tag());

   set_transform(node);
   if(!node.has_property("size"))throw logic_error("Required attribute 'size' not found for " + node.tag());

   m_size  = node.get_property("size",1.0);
   m_center = ("true" == node.get_property("center","false"))? true : false;
}

xsquare::~xsquare()
{}

std::shared_ptr<clipper_profile> xsquare::create_clipper_profile(const carve::math::Matrix& t) const
{
   std::shared_ptr<polygon2d> poly = primitives2d::make_rectangle(m_size,m_size,m_center,t*get_transform());
   std::shared_ptr<clipper_profile> mesh(new clipper_profile());
   mesh->AddPaths(poly->paths());
   return mesh;
}


std::shared_ptr<carve::mesh::MeshSet<3>> xsquare::create_carve_mesh(const carve::math::Matrix& t) const
{
   std::shared_ptr<xpolyhedron> poly = primitives3d::make_cuboid(m_size,m_size, mesh_utils::thickness(),m_center,false,t*get_transform());
   return poly->create_carve_mesh();
}
