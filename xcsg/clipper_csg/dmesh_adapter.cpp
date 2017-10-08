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

#include "dmesh_adapter.h"

#include "dmesh/dmesh.h"
#include "dmesh/dvertex.h"
#include "dmesh/dtriangle.h"

dmesh_adapter::dmesh_adapter(double maxlen)
: m_maxlen(maxlen)
, m_mesh(new polymesh2d())
{}

dmesh_adapter::~dmesh_adapter()
{}

bool dmesh_adapter::tesselate(std::shared_ptr<polyset2d> polyset)
{
   dmesh dmesher;

   // transfer the contours from polyset to dmesher
   for(auto i=polyset->begin(); i!=polyset->end(); i++) {
      std::shared_ptr<polygon2d> poly = *i;
      add_contour(poly,dmesher);
   }

   // now we have all the contours. Perform tesselation based on the profile vertices only
   if(dmesher.triangulate_profile()) {

      // convert vertices to polymesh2d
      // vertex traversal (skip 3 first supervertices, they are supervertices)
      size_t nv =  dmesher.vertex_size();
      m_mesh->m_vert.reserve(nv);
      for(size_t iv=3; iv<nv; iv++) {
         const dvertex* v = dmesher.get_vertex(iv);
         const dpos2d& p = v->pos();
         m_mesh->m_vert.push_back(dpos2d(p.x(),p.y()));
      }

      // convert trangle faces to polymesh2d
      m_mesh->m_face.reserve(dmesher.size());
      for(auto triangle : dmesher ) {

         polymesh2d::index_vector face;
         face.reserve(3);

         // subtract 3 for supervertices in dmesh
         face.push_back(triangle->vertex1()-3);
         face.push_back(triangle->vertex2()-3);
         face.push_back(triangle->vertex3()-3);
         m_mesh->add_face(face);
      }

      // convert contours to polymesh2d
      const dprofile* profile = dmesher.get_profile();
      m_mesh->m_contour.reserve(profile->size());
      for(auto loop : *profile) {

         // vertex vector for contour
         polymesh2d::index_vector vind;
         vind.reserve(loop->size());

         for(auto coedge : *loop) {
            size_t iv = coedge->vertex1();
            vind.push_back(iv-3);
         }
         m_mesh->m_contour.push_back(vind);
      }
      return true;
   }

   throw std::logic_error("dmesh_adapter: tesselation failed ");

   return false;
}

bool dmesh_adapter::add_contour(std::shared_ptr<polygon2d> poly, dmesh& dmesher)
{
   size_t ncontour = poly->size();
   for(size_t icontour=0; icontour<ncontour; icontour++) {

      std::shared_ptr<const contour2d> contour = (*poly)[icontour];

      // add the loop to dmesh, first the list of positions
      std::vector<dpos2d> loop = build_loop_points(contour);

      // Then add the loop to dmesh
      dmesher.add_loop(loop);
   }

   return true;
}

std::vector<dpos2d> dmesh_adapter::build_loop_points(std::shared_ptr<const contour2d> contour)
{
   std::vector<dpos2d> loop;

   size_t nv = contour->size();
   loop.reserve(nv*2);
   for(size_t i=0; i<nv;i++) {
      const dpos2d& vtx = (*contour)[i];

      if( (m_maxlen>0.0) && (i>0) ) {
         // because of dmesh limitation we may limit the length of
         // loop edges to get a good/correct mesh.
         // If length is exceeded we compute interpolated points

         // check if contour segment length is longer than m_maxlen
         const dpos2d& vtx_prev = (*contour)[i-1];
         dvec2d dir(vtx_prev,vtx);
         double length = dir.length();
         if(length > m_maxlen) {
            // number of intermediate points required and length of new segments
            size_t nseg = size_t(length/m_maxlen);
            double dlen = length/nseg;
            size_t np   = nseg-1;
            dir.normalise();

            // generate intermediate points
            for(size_t ip=0; ip<np; ip++) {
               dpos2d p = vtx_prev + (ip+1)*dlen*dir;
               loop.push_back(p);
            }
         }
      }

      // push the contour point
      loop.push_back(dpos2d(vtx.x(),vtx.y()));
   }

   // final edge
   if( m_maxlen>0.0 ) {
      // because of dmesh limitation we may limit the length of
      // loop edges to get a good/correct mesh.
      // If length is exceeded we compute interpolated points

      // check if contour segment length is longer than m_maxlen
      const dpos2d& vtx_prev = (*contour)[nv-1];
      const dpos2d& vtx      = (*contour)[0];
      dvec2d dir(vtx_prev,vtx);
      double length = dir.length();
      if(length > m_maxlen) {
         // number of intermediate points required and length of new segments
         size_t nseg = size_t(length/m_maxlen);
         double dlen = length/nseg;
         size_t np   = nseg-1;
         dir.normalise();

         // generate intermediate points
         for(size_t ip=0; ip<np; ip++) {
            dpos2d p = vtx_prev + (ip+1)*dlen*dir;
            loop.push_back(p);
         }
      }
   }


   loop.shrink_to_fit();
   return std::move(loop);
 }
