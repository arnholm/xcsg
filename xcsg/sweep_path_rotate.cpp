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

#include "sweep_path_rotate.h"
#include "mesh_utils.h"

static const double pi = 4.0*atan(1.0);

sweep_path_rotate::sweep_path_rotate(std::shared_ptr<const polymesh2d> pm2d, double angle, double pitch, int nseg)
: m_pm2d(pm2d)
, m_angle(angle)
, m_nseg(nseg)
, m_pitch(pitch)
{
   if(m_nseg < 1) {

      // estimate the max radius of the profile
      // also check if the profile width is greater than the pitch
      double radius = -1;
      double ymin = std::numeric_limits<double>::max();
      double ymax = std::numeric_limits<double>::min();
      for(size_t iv=0; iv<m_pm2d->nvertices(); iv++) {
         const dpos2d& vtx = m_pm2d->vertex(iv);
         radius = std::max(radius,fabs(vtx.x()));
         ymin = std::min(ymin,vtx.y());
         ymax = std::max(ymax,vtx.y());
      }

      if(fabs(pitch) > 0) {
         if(fabs(ymax-ymin) > fabs(pitch)) {
            throw std::logic_error("sweep_path_rotate: self intersection detected! Please make sure pitch is wider than swept profile");
         }
      }

      // start with a minimum of 2 segments and increase until the tolerance is satisfied
      nseg = 2;
      double alpha = m_angle/nseg;
      // guard against multiples of 2*pi
      // the resulting segment angle must be less than PI/2
      while( (fabs(alpha)>0.5*pi) || (radius*(1.0-cos(0.5*alpha)) > mesh_utils::secant_tolerance()) ) {
         nseg += 1;
         alpha = m_angle/nseg;
      }

      m_nseg = nseg;
   }
}

sweep_path_rotate::~sweep_path_rotate()
{}


std::shared_ptr<const polymesh3d> sweep_path_rotate::profile(double p) const
{
   // negative rotate about Y
  double angle = p*fabs(m_angle);

   // rotation matrix
   carve::math::Matrix t = carve::math::Matrix::ROT(angle,carve::geom::VECTOR(0.0,1.0,0.0));

/* below is eqivalent to ROT above
   carve::math::Matrix t = carve::math::Matrix();
   t.m[0][0] =  cos(angle);
   t.m[2][0] = -sin(angle);
   t.m[0][2] =  sin(angle);
   t.m[2][2] =  cos(angle);
*/

   if(fabs(m_pitch) > 0.0) {

      double dy    = m_pitch*angle/(2*pi);


      // obtain the Radius radius = 0.5*Dp of the center of the thread
      double xmin = std::numeric_limits<double>::max();
      double xmax = std::numeric_limits<double>::min();
      for(size_t iv=0; iv!=m_pm2d->nvertices(); iv++) {
         const dpos2d& vtx = m_pm2d->vertex(iv);
         xmax = std::max(xmax,vtx.x());
         xmin = std::min(xmin,vtx.x());
      }
      double radius = (xmax+xmin)/2.0;
      double angle_pitch = atan(m_pitch/(2*pi*radius));

      // tilt the profile around the global X axis to accomodate the pitch angle
      // notice the global sweep rotation axis is Y
      carve::math::Matrix rot_p = carve::math::Matrix::ROT(angle_pitch,carve::geom::VECTOR(1.0,0.0,0.0));

      // first rotate the pitch, then rotate around Y, then translate in Y
      t  =  carve::math::Matrix::TRANS(0.0,dy,0.0) * t  * rot_p ;
   };


   return std::shared_ptr<const polymesh3d>(new polymesh3d(m_pm2d,t));
}

size_t sweep_path_rotate::nseg() const
{
   return std::max(1,m_nseg);
}
