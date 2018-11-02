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


#ifndef CSPLINES_SPLINE_PATH_H
#define CSPLINES_SPLINE_PATH_H

#include "csplines_config.h"
#include "ap/ap.h"
#include <memory>
#include <vector>

// spline_path expresses a spline curve in 3d space
// so that (x,y,z) = pos(t), where t=[0,1]
// t=0 corresponds to first point given
// t=1 corresponds to last point given

namespace csplines {

   // tiny struct to represent cubic spline control point
   struct CSPLINES_PUBLIC cpoint {
      cpoint(double px_, double py_, double pz_, double vx_, double vy_, double vz_) : px(px_), py(py_), pz(pz_),vx(vx_),vy(vy_),vz(vz_) {}
      inline double dist(const cpoint& other) const {  double dx(px-other.px),dy(py-other.py),dz(pz-other.pz); return sqrt(dx*dx+dy*dy+dz*dz); }
      inline double length() const { return sqrt(vx*vx+vy*vy+vz*vz); }
      inline cpoint summed_point() const { return cpoint(px+vx,py+vy,pz+vz,0,0,0);}
      double px,py,pz;
      double vx,vy,vz;
   };

   class CSPLINES_PUBLIC spline_path {
   public:
      spline_path();
      spline_path(const std::vector<cpoint>& points);
      virtual ~spline_path();

      // compute the spline from a vector of points
      bool compute_spline(const std::vector<cpoint>& points);

      // return interpolated position and vector using parameter [0,1]
      cpoint pos(double t) const;

      // return interpolated direction vectors using parameter [0,1]
      // px,py,pz contains curve tangent
      // vx,vy,vz contains interpolated vector
      cpoint dir(double t) const;

      // return max curvature observed by sampling nseg segments
      // note that this evaluates points only (see summed_spline() to evaluate both)
      double max_curvature(int nseg) const;

      // return vector scaling range, largest minus smallest
      double scaling_range() const;

      // return sum of segment lengths
      double length() const;

      // return number of input control points
      size_t size() const;

      // return equivalent spline where points and vectors are summed
      std::shared_ptr<csplines::spline_path> summed_spline() const;

      // return absolute curvature at parameter t [0,1]
      // NOTE: use only with summed spline, because vectors are not evaluated
      double curvature(double t) const;

   protected:
      std::vector<cpoint>  summed_points() const;

   private:
      std::vector<cpoint> m_points;
      double              m_length;  // sum of segment lengths
      ap::real_1d_array   m_cpx;
      ap::real_1d_array   m_cpy;
      ap::real_1d_array   m_cpz;
      ap::real_1d_array   m_cvx;
      ap::real_1d_array   m_cvy;
      ap::real_1d_array   m_cvz;
   };

}

#endif // CSPLINES_SPLINE_PATH_H
