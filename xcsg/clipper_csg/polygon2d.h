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

#ifndef POLYGON2D_H
#define POLYGON2D_H

#include "contour2d.h"
#include <vector>
#include <memory>

// A polygon2d is the most general 2d primitive, it can describe any
// 2d shape, and is the basis for 2d booleans.
// The polygon2d is defined using a vector of closed contours. The
// first contour is the polygon outer contour, any following contours
// represent holes. Some rules apply:
// - outer contour: vertices CCW as seen from +Z
// - hole contours: vertices CW (opposite outer contour)


class polygon2d {
public:
   polygon2d();
   virtual ~polygon2d();

   // reserve space for number of contours
   void reserve(size_t ncontour);

   // add a contour to the back of the contour vector
   void push_back(std::shared_ptr<contour2d> contour);

   // return number of contours
   size_t size() const;
   std::shared_ptr<contour2d> get_contour(size_t i) const;

   // read access to selected contour [ 0,size() >
   std::shared_ptr<const contour2d> operator[](size_t i) const;

   // return Clipper paths for this polygon, for use in booleans
   std::shared_ptr<ClipperLib::Paths> paths() const;

   static bool make_compatible(polygon2d& a, polygon2d& b, double epspnt);

   // return bounding box of this polygon
   dbox2d bounding_box() const;

private:
   std::vector<std::shared_ptr<contour2d>> m_contours;
};

#endif // POLYGON2D_H
