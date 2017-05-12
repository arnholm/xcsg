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

#include <boost/date_time.hpp>

#include "carve_boolean.h"
#include "xpolyhedron.h"
//#include <carve/mesh_simplify.hpp>
#include "carve/mesh_simplify.hpp"
#include <iostream>
#include <carve/input.hpp>

#include "boolean_timer.h"
#include "mesh_utils.h"

carve_boolean::carve_boolean()
{}

carve_boolean::~carve_boolean()
{
   clear();
}

void carve_boolean::clear()
{
   m_meshset = 0;
}

size_t carve_boolean::compute( std::shared_ptr<carve::mesh::MeshSet<3>> b,  carve::csg::CSG::OP op)
{
   try {
      if(!m_meshset.get()) {
         m_meshset = b;
      }
      else {
         // the time runs only when an actual boolean is taking place
         boost::posix_time::ptime p1 = boost::posix_time::microsec_clock::universal_time();

         carve::csg::CSG  csg;
         m_meshset = std::shared_ptr<carve::mesh::MeshSet<3>>(csg.compute(m_meshset.get(),b.get(),op));

         boost::posix_time::time_duration  ptime_diff = boost::posix_time::microsec_clock::universal_time() - p1;
         double elapsed_sec = 0.001*ptime_diff.total_milliseconds();

         boolean_timer::singleton().add_elapsed(elapsed_sec);
      }
   }
   catch (std::exception& ex)
   {
      std::string msg = string(ex.what()) + " (originated in carve_boolean::compute)";
      throw std::runtime_error(msg);
   }
   return m_meshset->meshes.size();
}

size_t carve_boolean::size() const
{
   if(!m_meshset.get()) return 0;

   return  m_meshset->meshes.size();
}

std::shared_ptr<xpolyhedron> carve_boolean::create_manifold(size_t manifold_id) const
{
   std::shared_ptr<xpolyhedron>  poly = 0;

   if(manifold_id<m_meshset->meshes.size()) {

      carve::mesh::Mesh<3>* mesh = m_meshset->meshes[manifold_id];
      size_t nfaces = mesh->faces.size();

      // find the vertices referenced by the selected manifold
      std::map<carve::mesh::Face<3>::vertex_t*,size_t> vi_map;
      std::map<size_t,carve::mesh::Face<3>::vertex_t*> iv_map;
      for (size_t iface=0; iface<nfaces; iface++) {
         carve::mesh::Face<3>* face = mesh->faces[iface];
         std::vector<carve::mesh::Face<3>::vertex_t*> verts;
         face->getVertices(verts);
         for(size_t i=0;i<verts.size();i++) {
            carve::mesh::Face<3>::vertex_t* vertex = verts[i];
            if(vi_map.find(vertex) == vi_map.end()) {
               size_t index = vi_map.size();
               vi_map.insert(std::make_pair(vertex,index));
               iv_map.insert(std::make_pair(index,vertex));
            }
         }
      }

      // create polyhedron referencing only the relevant vertices
      poly = std::shared_ptr<xpolyhedron>(new xpolyhedron());
      poly->v_reserve(vi_map.size());
      poly->f_reserve(nfaces);
      for(auto i=iv_map.begin(); i!=iv_map.end();i++) {
        carve::mesh::Face<3>::vertex_t* vtx = i->second;
        poly->v_add(vtx->v);
      }

      // recompute face vertex indices
      for (size_t iface=0; iface<nfaces; iface++) {
         carve::mesh::Face<3>* face = mesh->faces[iface];
         std::vector<carve::mesh::Face<3>::vertex_t*> verts;
         face->getVertices(verts);
         std::vector<size_t> indices;
         indices.reserve(verts.size());
         for(size_t i=0;i<verts.size();i++) {
            carve::mesh::Face<3>::vertex_t* vertex = verts[i];
            indices.push_back(vi_map[vertex]);
         }
         poly->f_add(xface(indices),false);
      }
   }
   return poly;
}

std::shared_ptr<carve::mesh::MeshSet<3>> carve_boolean::mesh_set()
{
   return m_meshset;
}

bool carve_boolean::merge_faces(double min_normal_angle)
{
 //   carve::mesh::MeshSimplifier simplifier;
 //   simplifier.mergeCoplanarFaces(m_meshset.get(), min_normal_angle);
    return true;
}

size_t  carve_boolean::eliminate_short_edges(double min_length)
{
   // carve::mesh::MeshSimplifier simplifier;
   // return simplifier.eliminateShortEdges(m_meshset.get(),min_length);
   return 0;
}

size_t carve_boolean::compute(qhull3d& qhull)
{
   // compute the convex 3d hull
   qhull.compute();

   // the result will now be tidy with all faces properly oriented

   // create a carve mesh from the qhull result so we can perform other booleans

   // vertices
   carve::input::PolyhedronData data;
   data.reserveVertices(static_cast<int>(qhull.nvertices()));
   for(auto iv=qhull.vertex_begin();iv!=qhull.vertex_end(); iv++) {
      qhull3d::xyz& v = iv->second;
      data.addVertex(carve::geom::VECTOR(v.x,v.y,v.z));
   }

   // faces
   data.reserveFaces(static_cast<int>(qhull.nfaces()),3);
   for(auto iface=qhull.face_begin(); iface!=qhull.face_end(); iface++) {

      // face vertices have already been oriented with normals out
      qhull3d::face_vertices& fv = iface->second;
      data.addFace(fv.begin(),fv.end());
   }

   // create the mesh
   carve::input::Options options;
   m_meshset = std::shared_ptr<carve::mesh::MeshSet<3>>(data.createMesh(options));

   //
   return 1;
}
