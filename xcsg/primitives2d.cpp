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
   
#include "primitives2d.h"
#include "mesh_utils.h"

static const double pi = 4.0*atan(1.0);

primitives2d::primitives2d()
{}

primitives2d::~primitives2d()
{}


std::shared_ptr<polygon2d> primitives2d::make_circle(double r, int nseg, const carve::math::Matrix& t)
{
   if(nseg < 0) {
      nseg = 4;
      double alpha = 2.0*pi/nseg;
      while(r*(1.0-cos(0.5*alpha)) >  mesh_utils::secant_tolerance()) {
         nseg += 2;
         alpha = 2*pi/nseg;
      }
   }
   double dang = 2*pi/nseg;

   std::shared_ptr<polygon2d> polygon(new polygon2d());
   std::shared_ptr<contour2d> contour(new contour2d());
   polygon->push_back(contour);

   contour->reserve(nseg);
   for(int iseg=0; iseg<nseg; iseg++) {
      double ang = iseg*dang;
      double x   = r*cos(ang);
      double y   = r*sin(ang);
      double z   = 0.0;

      // transform the vertex, drop the z coordinate afterwards. This causes projection
      xvertex v  = t * carve::geom::VECTOR(x,y,z);
      contour->push_back(dpos2d(v.x,v.y));
   }
   return polygon;
}


std::shared_ptr<polygon2d> primitives2d::make_rectangle(double dx, double dy,  bool center, const carve::math::Matrix& t)
{

   double tdx = (center)? -0.5 : 0.0;
   double tdy = (center)? -0.5 : 0.0;

   carve::math::Matrix tloc = carve::math::Matrix::TRANS(tdx,tdy,0);

   // scale up the cuboid and apply the user transform
   tloc = t * carve::math::Matrix::SCALE(dx,dy,1) * tloc;

   std::shared_ptr<polygon2d> polygon(new polygon2d());
   std::shared_ptr<contour2d> contour(new contour2d());
   polygon->push_back(contour);

   xvertex v0  = tloc * carve::geom::VECTOR(0,0,0);
   xvertex v1  = tloc * carve::geom::VECTOR(1,0,0);
   xvertex v2  = tloc * carve::geom::VECTOR(1,1,0);
   xvertex v3  = tloc * carve::geom::VECTOR(0,1,0);
   contour->push_back(dpos2d(v0.x,v0.y));
   contour->push_back(dpos2d(v1.x,v1.y));
   contour->push_back(dpos2d(v2.x,v2.y));
   contour->push_back(dpos2d(v3.x,v3.y));

   return polygon;
}


std::shared_ptr<polygon2d> primitives2d::make_polygon(const std::vector<xvertex>& vertices, const carve::math::Matrix& t)
{
   std::shared_ptr<polygon2d> polygon(new polygon2d());
   std::shared_ptr<contour2d> contour(new contour2d());
   polygon->push_back(contour);

   size_t nvert = vertices.size();
   contour->reserve(nvert);
   for(size_t ivert=0; ivert<nvert; ivert++) {

      xvertex v3d = vertices[ivert];

      // transform the vertex, drop the z coordinate afterwards. This causes projection
      xvertex v  = t * carve::geom::VECTOR(v3d.x,v3d.y,v3d.z);
      contour->push_back(dpos2d(v.x,v.y));
   }
   return polygon;
}
