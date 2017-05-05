// BeginLicense:
// Part of: dmesh - Delaunay mesh library
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
   
#ifndef dloop_optimizer_H
#define dloop_optimizer_H

#include <vector>
#include "dpos2d.h"

// dloop_optimizer is a stand alone optimizer intended for optimising points
// such as those returned from a dloop (which is always closed).
//
// The original vector of points is copied to a vector with fewer entries.
// Thus it works like a filter, removing some of the original points, adding no new points.
//
// Points are erased on the basis of the "arrow tolerance", i.e. for
// 3 consecutive points, p1 -- p2 -- p3 how far from the line p1-p3 is p2 allowed to be.

class dloop_optimizer {
public:

   dloop_optimizer(double arrow_max, double dist_max);
   virtual ~dloop_optimizer();

   // run the optimizer
   std::vector<dpos2d> optimize(const std::vector<dpos2d>& points) const;

   // compute signed area of ppoint loop
   static double signed_area(const std::vector<dpos2d>& points);

   double arrow_max() const { return m_arrow_max; }
   double dist_max() const { return m_dist_max; }

private:
   double m_arrow_max;  // "arrow tolerance" - max arrow distance from p2 to projection p1 - p3
   double m_dist_max;   // "distance tolerance" - max distance between vertices along loop
};

#endif // dloop_optimizer_H
