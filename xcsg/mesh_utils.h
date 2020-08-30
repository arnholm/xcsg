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

#ifndef MESH_UTILS_H
#define MESH_UTILS_H

#include <carve/matrix.hpp>

class mesh_utils {
public:

   // default mesh thickness of 2d shapes when using carve mesh for 2d
   static double thickness() { return 1.0; }

   // Factor expressing max edge length as fraction of profile dimention
   static double maxlen_factor() { return 0.3; }

   // tolerances for adaptive meshing of circular curves/surfaces
   // The tolerance measures the distance from a segment chord to the true circular curve, i.e.  radius*(1-cos(angle/2))
   static double secant_tolerance();
   static void set_secant_tolerance(double tol);

   static bool is_left_hand(const carve::math::Matrix& t);

private:
   static double m_secant_tolerance;
};

#endif // MESH_UTILS_H
