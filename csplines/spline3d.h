// BeginLicense:
// Part of: csplines - cubic splines library
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


#ifndef CSPLINES_SPLINE3D_H
#define CSPLINES_SPLINE3D_H

#include "csplines_config.h"
#include "ap/ap.h"
#include <vector>

// spline3d expresses a spline curve in 3d space
// so that (x,y,z) = pos(t), where t=[0,1]
// t=0 corresponds to first point given
// t=1 corresponds to last point given

namespace csplines {

   // tiny struct to represent cubic spline control point
   struct CSPLINES_PUBLIC cpoint {
      cpoint(double x_, double y_, double z_) : x(x_), y(y_), z(z_),xn(0.0),yn(0.0),zn(0.0) {}
      cpoint(double x_, double y_, double z_, double xn_, double yn_, double zn_) : x(x_), y(y_), z(z_),xn(xn_),yn(yn_),zn(zn_) {}
      inline double dist(const cpoint& p) const {  double dx(x-p.x),dy(y-p.y),dz(x-p.z); return sqrt(dx*dx+dy*dy+dz*dz); }
      double x,y,z;
      double xn,yn,zn;
   };

   class CSPLINES_PUBLIC spline3d {
   public:
      spline3d();
      spline3d(const std::vector<cpoint>& points);
      virtual ~spline3d();

      // compute the spline from a vector of points
      bool compute_spline(const std::vector<cpoint>& points);

      // return position using parameter [0,1]
      cpoint pos(double t) const;

      // return 1st derivative vector using parameter [0,1]
      cpoint deriv1(double t) const;

      // return max curvature observed by sampling nseg segments
      double max_curvature(int nseg) const;

   private:
      ap::real_1d_array m_cx;
      ap::real_1d_array m_cy;
      ap::real_1d_array m_cz;
      ap::real_1d_array m_cxn;
      ap::real_1d_array m_cyn;
      ap::real_1d_array m_czn;
   };

}

#endif // CSPLINES_SPLINE3D_H
