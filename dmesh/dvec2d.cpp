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

#include "dvec2d.h"
#include "dpos2d.h"
#include <math.h>

dvec2d::dvec2d(double x,double y)
: m_x(x),m_y(y)
{}

dvec2d::dvec2d()
: m_x(0.0),m_y(0.0)
{}

dvec2d::dvec2d(const dvec2d& vec)
: m_x(vec.m_x),m_y(vec.m_y)
{}

dvec2d::dvec2d(const dpos2d& start,const dpos2d& end)
: m_x(end.x()-start.x()),m_y(end.y()-start.y())
{}

dvec2d::~dvec2d()
{
}

double dvec2d::x() const
{
   return m_x;
}

double dvec2d::y() const
{
   return m_y;
}

dvec2d& dvec2d::operator=(const dvec2d& vec)
{
   m_x = vec.m_x;
   m_y = vec.m_y;
   return *this;
}

int dvec2d::operator<(const dvec2d& vec) const
{
   double mydist2    = m_x*m_x + m_y*m_y;
   double otherdist2 = vec.m_x*vec.m_x + vec.m_y*vec.m_y;
   return (mydist2<otherdist2);
}

dvec2d& dvec2d::normalise()
{
   double len = length();
   if(len > 0.0) {
      m_x = m_x/len;
      m_y = m_y/len;
   }
   return *this;
}

dvec2d& dvec2d::scale(double factor)
{
   m_x = m_x * factor;
   m_y = m_y * factor;
   return *this;
}

dvec2d& dvec2d::add(const dvec2d& vec2)
{
   m_x = m_x + vec2.m_x;
   m_y = m_y + vec2.m_y;
   return *this;
}

dvec2d& dvec2d::subtract(const dvec2d& vec2)
{
   m_x = m_x - vec2.m_x;
   m_y = m_y - vec2.m_y;
   return *this;
}

dvec2d& dvec2d::reverse()
{
   m_x = - m_x;
   m_y = - m_y;
   return *this;
}


double dvec2d::dot(const dvec2d& vec2) const
{
   return m_x*vec2.m_x + m_y*vec2.m_y;
}

double dvec2d::cross(const dvec2d& vec2) const
{
   return m_x*vec2.y()-vec2.x()*m_y;
}

double dvec2d::angle(const dvec2d& vec2) const
{
   dvec2d vector1(m_x,m_y);
   dvec2d vector2(vec2.m_x,vec2.m_y);
   vector1.normalise();
   vector2.normalise();
   double dotVal = vector1.dot( vector2 );
   if (dotVal>1.0) dotVal = 1.0;
   if (dotVal<-1.0) dotVal = -1.0;
   return acos( dotVal );
}

double dvec2d::length() const
{
   return sqrt(m_x*m_x + m_y*m_y);
}

double dvec2d::length_squared() const
{
   return (m_x*m_x + m_y*m_y);
}

dvec2d  dvec2d::normal(int sign) const
{
   if(sign<0) return dvec2d(-m_y,m_x);
   else return dvec2d(m_y,-m_x);
}

dvec2d& dvec2d::operator+=(const dvec2d& vec)
{
   m_x+=vec.x();
   m_y+=vec.y();
   return *this;
}

dvec2d& dvec2d::operator-=(const dvec2d& vec)
{
   m_x-=vec.x();
   m_y-=vec.y();
   return *this;
}

double dvec2d::operator%=(const dvec2d& vec)
{
   return m_x*vec.x()+m_y*vec.y();
}

dvec2d& dvec2d::operator*=(double factor)
{
   m_x*=factor;
   m_y*=factor;
   return *this;
}

dvec2d dvec2d::operator -() const
{
   return dvec2d(-m_x,-m_y);
}

dvec2d dvec2d::operator+(const dvec2d& vec)
{
   return dvec2d(m_x+vec.m_x,m_y+vec.m_y);
}

dvec2d dvec2d::operator-(const dvec2d& vec)
{
   return dvec2d(m_x-vec.m_x,m_y-vec.m_y);
}
