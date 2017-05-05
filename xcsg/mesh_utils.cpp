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

#include "mesh_utils.h"
#include <iostream>

double  mesh_utils::m_secant_tolerance = 0.05;
static const double min_secant_tolerance = 0.0009;

double mesh_utils::secant_tolerance()
{
   return m_secant_tolerance;
}

void  mesh_utils::set_secant_tolerance(double tol)
{
   if(tol > min_secant_tolerance) {
      m_secant_tolerance = tol;
   }
   else {
      std::cout << "Info: ignored secant tolerance " << tol << " < min tolerance=" << min_secant_tolerance << std::endl;
   }
}


bool mesh_utils::is_left_hand(const carve::math::Matrix& t)
{
   typedef carve::geom::vector<3> vec3d;
   vec3d x = carve::geom::VECTOR(t.m[0][0],t.m[1][0],t.m[2][0]);
   vec3d y = carve::geom::VECTOR(t.m[0][1],t.m[1][1],t.m[2][1]);
   vec3d z = carve::geom::VECTOR(t.m[0][2],t.m[1][2],t.m[2][2]);
   vec3d z_test = carve::geom::cross(x,y);
   double dot   = carve::geom::dot(z,z_test);
   return (dot < 0.0);
}
