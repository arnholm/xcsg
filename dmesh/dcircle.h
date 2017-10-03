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

#ifndef DCIRCLE_H
#define DCIRCLE_H

#include "dpos2d.h"

// a dcircle is mainly used for representing the circumcircle of a triangle in Delaunay meshing

class dcircle {
public:

   // construct from centre and radius
   dcircle(const dpos2d& center, double radius);

   // construct circumcircle from triangle points
   dcircle(const dpos2d& p1, const dpos2d& p2,const dpos2d& p3);

   virtual ~dcircle();

   // compute the circumcircle centre and radius from the 3 points
   static void circum_circle(const dpos2d& p1, const dpos2d& p2,const dpos2d& p3, dpos2d& center, double& radius);

   // return center and radius of circle
   const dpos2d& center() const    { return m_center; }
   double radius() const           { return m_radius; }

   // check if position is inside circle
   bool pos_inside(const dpos2d& p) const { return (m_center.dist(p) <= m_radius); }

private:
   dpos2d m_center;
   double m_radius;
};

#endif // DCIRCLE_H
