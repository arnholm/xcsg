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

#include "dpos2d.h"
#include <math.h>

dpos2d::dpos2d(double x,double y)
: m_x(x),m_y(y)
{}

dpos2d::dpos2d()
: m_x(0.0),m_y(0.0)
{}

dpos2d::dpos2d(const dpos2d& pos)
: m_x(pos.m_x),m_y(pos.m_y)
{}

dpos2d::~dpos2d()
{
}

void dpos2d::x(double x)
{
   m_x = x;
}

void dpos2d::y(double y)
{
   m_y = y;
}

dpos2d& dpos2d::operator=(const dpos2d& pos)
{
   m_x = pos.m_x;
   m_y = pos.m_y;
   return *this;
}

double dpos2d::dist(const dpos2d& pos) const
{
   double dx = pos.m_x - m_x;
   double dy = pos.m_y - m_y;
   return sqrt(dx*dx + dy*dy);
}

double dpos2d::dist_squared(const dpos2d& pos) const
{
   double dx = pos.m_x - m_x;
   double dy = pos.m_y - m_y;
   return (dx*dx + dy*dy);
}

dpos2d& dpos2d::operator+=(const dvec2d& vec)
{
    m_x+=vec.x();
    m_y+=vec.y();
    return *this;
}

dpos2d& dpos2d::operator-=(const dvec2d& vec)
{
    m_x-=vec.x();
    m_y-=vec.y();
    return *this;
}

dpos2d& dpos2d::operator+=(const dpos2d& pos)
{
    m_x+=pos.x();
    m_y+=pos.y();
    return *this;
}

dpos2d& dpos2d::operator-=(const dpos2d& pos)
{
    m_x-=pos.x();
    m_y-=pos.y();
    return *this;
}
