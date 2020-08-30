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

#include "sweep_mesh.h"

#include "mesh_utils.h"
#include <carve/matrix.hpp>


sweep_mesh::sweep_mesh(std::shared_ptr<sweep_path> path, bool torus)
: m_path(path)
, m_polyhedron(new xpolyhedron())
, m_torus(torus)
{}

sweep_mesh::~sweep_mesh()
{}

std::shared_ptr<xpolyhedron> sweep_mesh::polyhedron()
{
   if(m_polyhedron->v_size() == 0) {
      sweep();
   }
   return m_polyhedron;
}

bool sweep_mesh::sweep()
{
   // obtain the number of sweep segments and the parameter step length
   size_t nseg = m_path->nseg();
   double dp = 1.0/double(nseg);

   // traverse the sweep segments, i.e. parameter space p=[0,1]
   size_t last_seg = nseg-1;
   for(size_t iseg=0; iseg<nseg; iseg++) {

      // obtain the transformed profile mesh at parameter value p, with all 3d coordinates already computed
      double p = iseg*dp;
      std::shared_ptr<const polymesh3d> mesh = m_path->profile(p);

      // number of faces so far
      // size_t f_offset  = m_polyhedron->f_size();

      // offset to layer bottom vertices
      size_t v_offset0 = m_polyhedron->v_size();
      // add vertices for this layer by copying from the mesh
      add_mesh_vertices(mesh);
      // offset to layer top vertices can now be obtained
      size_t v_offset1 =  m_polyhedron->v_size();

      if(!m_torus && (iseg==0)) {
         // 1st segment, add flipped faces from mesh
         add_mesh_faces(v_offset0,mesh,true);
      }

      if(m_torus && (iseg==last_seg)) {
         // connect side faces to bottom layer to create a topological torus
         v_offset1 = 0;
      }

      // create side faces
      create_side_faces(v_offset0,v_offset1,mesh,false);
   }

   if(!m_torus) {
      // add normally oriented faces on top
      double p = 1.0;
      std::shared_ptr<const polymesh3d> mesh = m_path->profile(p);
      size_t v_offset0 = m_polyhedron->v_size();
      add_mesh_vertices(mesh);
      add_mesh_faces(v_offset0, mesh,false);
   }

   // the polyhedron should now be complete

   return true;
}

void sweep_mesh::add_mesh_vertices(std::shared_ptr<const polymesh3d> mesh)
{
   for(size_t iv=0; iv<mesh->nvertices(); iv++) {
      m_polyhedron->v_add(mesh->vertex(iv));
   }
}

void sweep_mesh::add_mesh_faces(size_t v_offset, std::shared_ptr<const polymesh3d> mesh, bool reverse)
{
   for(size_t i=0; i<mesh->nfaces(); i++) {

      // take a copy here
      polymesh3d::index_vector vinds = mesh->face(i);
      for(size_t i=0;i<vinds.size();i++) {
         vinds[i] += v_offset;
      }

      // add face indicies with proper vertex offset
      m_polyhedron->f_add(vinds,reverse);
   }
}


void sweep_mesh::create_side_faces(size_t v_offset0, // vertex offset to bottom layer vertices
                                   size_t v_offset1, // vertex offset to top layer vertices
                                   std::shared_ptr<const polymesh3d> mesh, bool reverse)
{
   // v_offset = offset to 1st vertex in vertex layer below the faces to be created

   for(size_t i=0; i<mesh->ncontours(); i++) {

      // zero level vertex indicies for this contour
      const polymesh3d::index_vector& vinds = mesh->contour(i);
      size_t nvc = vinds.size();

      // we must create nvc faces, where the last face connects to the first
      for(size_t ivc=0; ivc<nvc; ivc++) {

         size_t iv0 = v_offset0 + vinds[ivc];
         size_t iv1 = v_offset0 + ((ivc==(nvc-1))? vinds[0] : vinds[ivc+1]) ;
         size_t iv2 = iv1 + (v_offset1 - v_offset0);
         size_t iv3 = iv0 + (v_offset1 - v_offset0);
         m_polyhedron->f_add( {iv0,iv1,iv2,iv3}, reverse );
      }
   }
}
