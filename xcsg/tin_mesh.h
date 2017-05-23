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

#ifndef TIN_MESH_H
#define TIN_MESH_H

#include <memory>
#include <vector>
class dmesh;

class tin_mesh {
public:

   struct txyz {
      txyz(double tx,double ty, double tz): x(tx),y(ty),z(tz) {}
      double x,y,z;
   };

   typedef std::vector<size_t>  pface;
   struct tpoly {
      tpoly(const std::vector<txyz>& vertices) : m_vert(vertices) {}
      std::vector<txyz>   m_vert;
      std::vector<pface>  m_face;
   };

   // perform inital meshing of original points, returns non-closed mesh
   static std::shared_ptr<tpoly> make_tin(std::shared_ptr<tpoly> poly);

private:
   // convert non-closed mesh to closed polyhedron
   static std::shared_ptr<tpoly> close_tin(std::shared_ptr<tpoly> poly, dmesh& mesh);

};

#endif // TIN_MESH_H
