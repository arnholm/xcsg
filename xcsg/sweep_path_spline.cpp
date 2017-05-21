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

#include "sweep_path_spline.h"
#include "mesh_utils.h"

// #include <iostream>

static const double pi = 4.0*atan(1.0);

sweep_path_spline::sweep_path_spline(std::shared_ptr<const polymesh2d> pm2d, std::shared_ptr<const csplines::spline_path> path, int nseg)
: m_pm2d(pm2d)
, m_path(path)
, m_nseg(nseg)
{
   if(m_nseg < 1) {

      // start with a minimum of 2 segments and increase until the tolerance is satisfied
      nseg = 2;

      // estimate the max radius of the path
      int nseg_sample = 20;
      double c_max = path->max_curvature(nseg_sample);
  //    std::cout << "cmax=" << c_max << std::endl;

      if(c_max > 0.0) {

         double radius = 1.0/c_max;

         double angle  = 0.5*pi;   // Assuming 90 degree segment at given radius

         double alpha = angle/nseg;
         while( radius*(1.0-cos(0.5*alpha)) > mesh_utils::secant_tolerance() ) {
            nseg += 1;
            alpha = angle/nseg;
         }

         // we now have number of segments for a presumed 90 degree circular path at given radius
         // scale actual number of segments by known path length

         double arclen = 0.5*pi*radius;
         double slen   = path->length();

         nseg *= slen/arclen;

        //  std::cout << "radius=" << radius << ", nseg=" << nseg <<  std::endl;

      }

      m_nseg = nseg;
   }
}

sweep_path_spline::~sweep_path_spline()
{}


std::shared_ptr<const polymesh3d> sweep_path_spline::profile(double p) const
{
   typedef carve::geom::vector<3> vec3d;
   const vec3d xglob = carve::geom::VECTOR( 1, 0, 0);
   const vec3d yglob = carve::geom::VECTOR( 0, 1, 0);
   const vec3d zglob = carve::geom::VECTOR( 0, 0, 1);

   carve::math::Matrix t = carve::math::Matrix::IDENT();

   // get the curve point
   csplines::cpoint cp = m_path->pos(p);

   size_t icol = 3;
   t.m[3][0] = cp.px;
   t.m[3][1] = cp.py;
   t.m[3][2] = cp.pz;

   // get the profile normal vector at parameter p
   csplines::cpoint cp_dir = m_path->dir(p);

   vec3d zdir = carve::geom::VECTOR( cp_dir.px, cp_dir.py, cp_dir.pz).normalize();
   vec3d ydir = carve::geom::VECTOR( cp_dir.vx, cp_dir.vy, cp_dir.vz);

   if(!(ydir.length() > 0.0) ) {
      // ydir must be guesstimated
      ydir = carve::geom::cross(zdir,yglob).normalize();
      if(!(ydir.length() > 0.0) ) {
         ydir = carve::geom::cross(zdir,zglob).normalize();
      }
   }

   vec3d xdir = carve::geom::cross(ydir,zdir).normalize();
   ydir       = carve::geom::cross(zdir,xdir).normalize();

   t.m[0][0] = xdir[0];
   t.m[0][1] = xdir[1];
   t.m[0][2] = xdir[2];

   t.m[1][0] = ydir[0];
   t.m[1][1] = ydir[1];
   t.m[1][2] = ydir[2];

   t.m[2][0] = zdir[0];
   t.m[2][1] = zdir[1];
   t.m[2][2] = zdir[2];

   return std::shared_ptr<const polymesh3d>(new polymesh3d(m_pm2d,t));
}


size_t sweep_path_spline::nseg() const
{
   return std::max(1,m_nseg);
}
