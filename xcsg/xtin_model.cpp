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
   
#include "xtin_model.h"
#include "primitives3d.h"
#include <carve/input.hpp>
#include <map>
#include "tin_mesh.h"

xtin_model::xtin_model(const cf_xmlNode& const_node)
{
   if(const_node.tag() != "tin_model")throw logic_error("Expected xml tag tin_model, but found " + const_node.tag());

   set_transform(const_node);

   cf_xmlNode node = const_node;
   for(auto i=node.begin(); i!=node.end(); i++) {

      cf_xmlNode sub(i);
      if(!sub.is_attribute_node()) {

         if("vertices" == sub.tag()) {

            map<size_t,xvertex> v;
            for(auto iv=sub.begin(); iv!=sub.end(); iv++) {
               // get next vertex
               cf_xmlNode vertex(iv);
               if("vertex" == vertex.tag()) {
                  size_t id = v.size();
                  v[id]= carve::geom::VECTOR( vertex.get_property("x",0.0),
                                              vertex.get_property("y",0.0),
                                              vertex.get_property("z",0.0));
               }
               else {
                  throw logic_error("tin_model: expected tag 'vertex' but found " + vertex.tag());
               }
            }

            // copy to member variable
            m_vertices.resize(v.size());
            for(auto i=v.begin(); i!=v.end(); i++) {
               m_vertices[i->first] = i->second;
            }
         }
      }
   }
}

xtin_model::~xtin_model()
{}


std::shared_ptr<carve::mesh::MeshSet<3>> xtin_model::create_carve_mesh(const carve::math::Matrix& t) const
{
   // create a neutral vector of vertices for tin_mesh
   std::vector<tin_mesh::txyz>  vert;
   vert.reserve(m_vertices.size());
   for(auto& v : m_vertices) {
      vert.push_back(tin_mesh::txyz(v.x,v.y,v.z));
   }

   // create neutral polyhedron with only vertices
   std::shared_ptr<tin_mesh::tpoly> poly(new tin_mesh::tpoly(vert));

   // perform meshing, this adds faces to poly
   poly = tin_mesh::make_tin(poly);

   // convert to carve mesh
   carve::input::PolyhedronData data;
   data.reserveVertices(static_cast<int>(m_vertices.size()));
   for(size_t i=0; i<m_vertices.size(); i++) {
      data.addVertex(t*get_transform()*m_vertices[i]);
   }

   data.reserveFaces(static_cast<int>(poly->m_face.size()),3);
   for(size_t i=0; i<poly->m_face.size(); i++) {
      const xface& face = poly->m_face[i];
      data.addFace(face.begin(),face.end());
   }

   carve::input::Options options;
   return std::shared_ptr<carve::mesh::MeshSet<3>>(data.createMesh(options));
}
