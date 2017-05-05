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
   
#ifndef CARVE_BOOLEAN_H
#define CARVE_BOOLEAN_H

#include <vector>
#include <memory>
class xpolyhedron;
#include <carve/csg.hpp>
#include "qhull/qhull3d.h"

class carve_boolean {
public:

/*
   // carve::csg::CSG::OP
   enum OP {
     UNION,                  // in a or b.
     INTERSECTION,           // in a and b.
     A_MINUS_B,              // in a, but not b.
     B_MINUS_A,              // in b, but not a.
     SYMMETRIC_DIFFERENCE,   // in a or b, but not both.
     ALL                     // all split faces from a and b
   };
*/

   carve_boolean();
   virtual ~carve_boolean();

   // empty m_meshset
   void clear();

   // compute boolean against current mesh using a MeshSet as "b"
   size_t compute(std::shared_ptr<carve::mesh::MeshSet<3>> b, carve::csg::CSG::OP op);

   // special 3d hull computation
   size_t compute(qhull3d& qhull);

   // number of resulting manifolds
   size_t size() const;

   bool merge_faces(double min_normal_angle = 1.0e-2);

   size_t eliminate_short_edges(double min_length = 1.0e-1);

   // create result manifolds from mesh
   std::shared_ptr<xpolyhedron>  create_manifold(size_t imani) const;

   // return the current mesh
   std::shared_ptr<carve::mesh::MeshSet<3>> mesh_set();

private:
   std::shared_ptr<carve::mesh::MeshSet<3>> m_meshset;
};

#endif // CARVE_BOOLEAN_H
