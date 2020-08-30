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

#include "tin_mesh.h"
#include "dmesh/dmesh.h"
#include "dmesh/dtriangle.h"
#include <algorithm>
#include <iostream>

std::shared_ptr<tin_mesh::tpoly> tin_mesh::make_tin(std::shared_ptr<tpoly> poly)
{

   dmesh mesh;

   // convert 3d points to 2d
   std::vector<dpos2d> points;
   points.reserve(poly->m_vert.size());
   for( auto& v : poly->m_vert) {
      points.push_back(dpos2d(v.x,v.y));
   }

   // compute delaunay mesh
   mesh.triangulate_point_cloud(points);

   // get triangles
   poly->m_face.reserve(mesh.size());
   for(dtriangle* t : mesh) {
      size_t v1 = t->vertex1()-3;
      size_t v2 = t->vertex2()-3;
      size_t v3 = t->vertex3()-3;
      poly->m_face.push_back({v1,v2,v3});
   }

   return close_tin(poly,mesh);
}

std::shared_ptr<tin_mesh::tpoly> tin_mesh::close_tin(std::shared_ptr<tpoly> poly, dmesh& mesh)
{
   size_t nvert = poly->m_vert.size();
   size_t nface = poly->m_face.size();

   // compute min and max z
   double zmin = poly->m_vert[0].z;
   double zmax = zmin;
   for( auto& v : poly->m_vert) {
      zmin = std::min(zmin,v.z);
      zmax = std::max(zmax,v.z);
   }
   double diff = zmax-zmin;



   std::shared_ptr<tpoly> cpoly = std::shared_ptr<tpoly>(new tpoly( std::vector<txyz>()));
   cpoly->m_vert.reserve(nvert*2);
   cpoly->m_face.reserve(nface*2);

   // z coordinate of bottom vertices
   double zlow = zmin-diff*0.1;
   for( auto& v : poly->m_vert) cpoly->m_vert.push_back(txyz(v.x,v.y,zlow));  // bottom vertices
   for( auto& v : poly->m_vert) cpoly->m_vert.push_back(v);                   // top vertices


   for( auto& f : poly->m_face)  cpoly->m_face.push_back({f[2],f[1],f[0]});    // bottom faces (reversed)
   for( auto& f : poly->m_face) cpoly->m_face.push_back({f[0]+nvert,f[1]+nvert,f[2]+nvert}); // top faces


   return cpoly;
}
