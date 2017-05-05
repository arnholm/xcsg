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
   
#include "xpolygon.h"
#include "primitives2d.h"
#include "primitives3d.h"
#include "cf_xmlNode.h"
#include <map>

xpolygon::xpolygon()
{}

xpolygon::xpolygon(const cf_xmlNode& node)
{
   if(node.tag() != "polygon")throw logic_error("Expected xml tag 'polygon', but found " + node.tag());

   set_transform(node);

   cf_xmlNode vertices;
   if(node.get_child("vertices",vertices)) {

       // be sure to sort the vertices
       std::map<size_t,xvertex> v;
       for(auto iv=vertices.begin(); iv!=vertices.end(); iv++) {

          // get next vertex
          cf_xmlNode vertex(iv);
          if("vertex" == vertex.tag()) {
             size_t id = v.size();
             v[id]= carve::geom::VECTOR( vertex.get_property("x",0.0),
                                         vertex.get_property("y",0.0),
                                                                 0.0);
          }
          else {
             throw logic_error("polygon: expected tag 'vertex' but found " + vertex.tag());
          }
       }

       // copy to member variable
       m_vert.resize(v.size());
       for(auto i=v.begin(); i!=v.end(); i++) {
          m_vert[i->first] = i->second;
       }
   }
}

void   xpolygon::v_reserve(size_t nverts)
{
   m_vert.reserve(nverts);
}

size_t xpolygon::v_add(double x, double y)
{
   size_t index = m_vert.size();
   m_vert.push_back(carve::geom::VECTOR(x,y,0));
   return index;
}

size_t xpolygon::v_size() const
{
   return m_vert.size();
}

xpolygon::~xpolygon()
{}

std::shared_ptr<clipper_profile> xpolygon::create_clipper_profile(const carve::math::Matrix& t) const
{
   std::shared_ptr<polygon2d> poly = primitives2d::make_polygon(m_vert,t*get_transform());
   std::shared_ptr<clipper_profile> mesh(new clipper_profile());
   mesh->AddPaths(poly->paths());
   return mesh;
}


std::shared_ptr<carve::mesh::MeshSet<3>> xpolygon::create_carve_mesh(const carve::math::Matrix& t) const
{
   std::shared_ptr<xpolyhedron> poly = primitives3d::make_polygon(m_vert, mesh_utils::thickness(),t*get_transform());
   return poly->create_carve_mesh();
}

