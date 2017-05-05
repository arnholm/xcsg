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
   
#include "tin_mesh.h"
#include "dmesh/dmesh.h"
#include "dmesh/dtriangle.h"

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

   return poly;
}
