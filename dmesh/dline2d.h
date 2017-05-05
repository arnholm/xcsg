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
   
#ifndef DLINE2D_H
#define DLINE2D_H

#include "dpos2d.h"
#include <utility>

// a straight line defined by to positions

class dline2d {
public:
	dline2d();
   dline2d(const dpos2d& end1, const dpos2d& end2);
   dline2d(const dline2d& line);
	virtual ~dline2d();

   dline2d& operator =(const dline2d& line);

   const dpos2d& end1() const;
   const dpos2d& end2() const;

   double length() const;

   /// parameter (0.0 at end1 and 1.0 at end2), i.e. relative distance from end1
   /// This function also allows extrapolation beyond line ends
   dpos2d  interpolate(double par) const;

   /// project point onto line and return parameter value (0.0 at end1 and 1.0 at end2)
   /// The function may return parameter outside the range [0,1]
   double project(const dpos2d& point) const;

   /// compute line/line intersection and return intersection point.
   /// Observe that intersection point may lie anywhere beyond the line endpoints.
   /// The function returns false if the lines are parallel
   bool   intersect(const dline2d& other_line, dpos2d& pos) const;

   /// compute line/line intersection and return intersection parameters in range
   /// If both parameters are in the range [0,1], there is an actual intersection of finite length segments.
   /// The function returns false if the lines are parallel
   bool   intersect(const dline2d& other_line, dpos2d& pos,double& this_param,double& other_param) const;

private:
   dpos2d m_end1;
   dpos2d m_end2;
};

#endif

