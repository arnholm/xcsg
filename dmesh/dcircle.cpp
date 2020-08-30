// BeginLicense:
// Part of: dmesh - Delaunay mesh library
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

#include "dcircle.h"
#include "dvec2d.h"
#include <cmath>

dcircle::dcircle(const dpos2d& center, double radius)
: m_center(center)
, m_radius(radius)
{}


dcircle::dcircle(const dpos2d& p1, const dpos2d& p2,const dpos2d& p3)
{
   circum_circle(p1,p2,p3,m_center,m_radius);
}

dcircle::~dcircle()
{
}

void dcircle::circum_circle(const dpos2d& p1, const dpos2d& p2,const dpos2d& p3, dpos2d& center, double& radius)
{
   // https://en.wikipedia.org/wiki/Circumscribed_circle#Cartesian_coordinates_from_cross-_and_dot-products

   dvec2d p1p2(p2,p1);
   dvec2d p2p3(p3,p2);
   dvec2d p3p1(p1,p3);

   dvec2d p1p3(p3,p1);
   dvec2d p2p1(p1,p2);
   dvec2d p3p2(p2,p3);

   double dp1p2 = p1p2.length();
   double dp2p3 = p2p3.length();
   double dp3p1 = p3p1.length();
   double dp1p3 = dp3p1;

   radius = fabs(0.5*(dp1p2*dp2p3*dp3p1)/p1p2.cross(p2p3));

   double cross = p1p2.cross(p2p3);
   double denom = 2.0* cross*cross;

   double alpha = dp2p3*dp2p3*p1p2.dot(p1p3)/denom;
   double beta  = dp1p3*dp1p3*p2p1.dot(p2p3)/denom;
   double gamma = dp1p2*dp1p2*p3p1.dot(p3p2)/denom;

   double x = alpha*p1.x() + beta*p2.x() + gamma*p3.x();
   double y = alpha*p1.y() + beta*p2.y() + gamma*p3.y();

   center = dpos2d(x,y);
}
