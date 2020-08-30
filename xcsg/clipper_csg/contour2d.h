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

#ifndef CONTOUR2D_H
#define CONTOUR2D_H

#include "clipper_csg_config.h"

#include "dmesh/dpos2d.h"
#include "dmesh/dbox2d.h"
#include <vector>

class contour2d {
public:
   contour2d();
   contour2d(const ClipperLib::Path& cpath);
   virtual ~contour2d();

   void clear();

   void reserve(size_t nvert);
   void push_back(const dpos2d& vertex);
   size_t size() const;
   const dpos2d& operator[](size_t i) const;

   // return length of contour
   double length() const;

   // compute geometric center
   dpos2d geometric_center() const;

   // make_compatible recomputes a and b to be compatible for use in "transform_extrude"
   static bool make_compatible(contour2d& a, contour2d& b, double epspnt);

   // compute the signed area of the contour
   // CCW contours have positive areas
   double signed_area() const;

   // reverse the winding order of the contour
   void reverse();

   // return bounding box of this contour
   dbox2d bounding_box() const;

   ClipperLib::Path path() const;
private:
   std::vector<dpos2d> m_vert;
};

#endif // CONTOUR2D_H
