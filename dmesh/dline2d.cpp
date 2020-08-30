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

#include "dline2d.h"
#include <math.h>
#include <limits>

dline2d::dline2d()
: m_end1(dpos2d(0.0,0.0)),m_end2(dpos2d(0.0,0.0))
{}

dline2d::dline2d(const dpos2d& end1, const dpos2d& end2)
: m_end1(end1),m_end2(end2)
{}

dline2d::dline2d(const dline2d& line)
: m_end1(line.m_end1),m_end2(line.m_end2)
{}

dline2d::~dline2d()
{}

dline2d& dline2d::operator =(const dline2d& line)
{
   m_end1 = line.m_end1;
   m_end2 = line.m_end2;
   return *this;
}

const dpos2d& dline2d::end1() const
{
   return m_end1;
}

const dpos2d& dline2d::end2() const
{
   return m_end2;
}

double dline2d::length() const
{
   return m_end1.dist(m_end2);
}

dpos2d dline2d::interpolate(double par) const
{
   // parameter in interval [0.0, 1.0], i.e. relative distance from end1
   double x = m_end1.x() + par * ( m_end2.x() - m_end1.x() );
   double y = m_end1.y() + par * ( m_end2.y() - m_end1.y() );
   return dpos2d(x,y);
}

double det(const dvec2d& ab,const dvec2d& cd) {
   return ab.x()*cd.y()-ab.y()*cd.x();
}

double det(const dpos2d& ab,const dpos2d& cd) {
   return ab.x()*cd.y()-ab.y()*cd.x();
}


double dline2d::project(const dpos2d& point) const
{
   dvec2d line_vec(m_end1,m_end2);
   dvec2d point_vec(m_end1,point);

   double qlen=line_vec.dot(line_vec);
   if(qlen<=0) return 0.0;

   double prd=line_vec.dot(point_vec);
   return prd/qlen;
}

bool dline2d::intersect(const dline2d& other_line, dpos2d& pos) const
{
   double below=det(dvec2d(end2(),end1()),dvec2d(other_line.end2(),other_line.end1()));

   //Lines are parallel
   if(fabs(below) > 0.0) {
      double line1det=det(end1(),end2());
      double line2det=det(other_line.end1(),other_line.end2());
      double above_x=det(dvec2d(line1det,end1().x()-end2().x()),dvec2d(line2det,other_line.end1().x()-other_line.end2().x()));

      double above_y=det(dvec2d(line1det,end1().y()-end2().y()),dvec2d(line2det,other_line.end1().y()-other_line.end2().y()));

      pos=dpos2d(above_x/below,above_y/below);
      return true;
   }
   return false;
}

bool dline2d::intersect(const dline2d& other_line, dpos2d& pos,double& this_param,double& other_param) const
{
   if(!intersect(other_line,pos)) return false;
   this_param  = project(pos);
   other_param = other_line.project(pos);
   return true;
}
