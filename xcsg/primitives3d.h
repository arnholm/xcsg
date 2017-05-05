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
   
#ifndef PRIMITIVES3D_H
#define PRIMITIVES3D_H

#include <memory>
#include <vector>
#include "xpolyhedron.h"
#include <carve/matrix.hpp>
#include "mesh_utils.h"

// factory for 3d primitives

class primitives3d {
public:
   // cube and cuboid
   static std::shared_ptr<xpolyhedron> make_cuboid(double dx, double dy, double dz, bool center_xy, bool center_z, const carve::math::Matrix& t = carve::math::Matrix());

   // cylinder and cone
   static std::shared_ptr<xpolyhedron> make_cone(double r1, double r2, double height, bool center, int nseg, const carve::math::Matrix& t = carve::math::Matrix());

   // sphere
   static std::shared_ptr<xpolyhedron> make_sphere(double r, int nseg, const carve::math::Matrix& t = carve::math::Matrix());

   // geodesic_sphere
   static std::shared_ptr<xpolyhedron> make_geodesic_sphere(double r, int nseg, const carve::math::Matrix& t = carve::math::Matrix());

   // polyhedron from polygon
   static std::shared_ptr<xpolyhedron> make_polygon(const std::vector<xvertex>& vertices, double dz, const carve::math::Matrix& t = carve::math::Matrix());

};

#endif // PRIMITIVES3D_H
