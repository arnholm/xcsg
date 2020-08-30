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

#ifndef PRIMITIVES2D_H
#define PRIMITIVES2D_H

#include "xpolygon.h"
#include "clipper_csg/polygon2d.h"

class primitives2d {
public:
   primitives2d();
   virtual ~primitives2d();

   // circle
   static std::shared_ptr<polygon2d> make_circle(double r, int nseg, const carve::math::Matrix& t = carve::math::Matrix());

   // rectangle and square
   static std::shared_ptr<polygon2d> make_rectangle(double dx, double dy, bool center, const carve::math::Matrix& t = carve::math::Matrix());

   // polygon without holes
   static std::shared_ptr<polygon2d> make_polygon(const std::vector<xvertex>& vertices, const carve::math::Matrix& t = carve::math::Matrix());

private:
};

#endif // PRIMITIVES2D_H
