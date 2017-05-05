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
   
// dbox2d.cpp: implementation of the dbox2d class.
//
//////////////////////////////////////////////////////////////////////

#include "dbox2d.h"
#include <algorithm>
using namespace std;

dbox2d::dbox2d(bool initialised)
: m_p1(0,0),m_p2(0,0),m_initialised(initialised)
{}

dbox2d::dbox2d(const dpos2d& pos1, const dpos2d& pos2)
:m_initialised(true)
{
   setBox(pos1,pos2);
}

dbox2d::~dbox2d()
{
}

dbox2d& dbox2d::operator =(const dbox2d& box)
{
   if(this!=&box) {
      m_p1 = box.m_p1;
      m_p2 = box.m_p2;
      m_initialised = box.m_initialised;
   }
   return *this;
}


void dbox2d::setBox(const dpos2d& pos1, const dpos2d& pos2)
{
   if (!m_initialised) {
      m_p1 = pos1;
      m_p2 = m_p1;
      m_initialised = true;
   }

   double lminx = min(pos1.x(),pos2.x());
   double lmaxx = max(pos1.x(),pos2.x());
   double lminy = min(pos1.y(),pos2.y());
   double lmaxy = max(pos1.y(),pos2.y());

   m_p1 = dpos2d(lminx,lminy);
   m_p2 = dpos2d(lmaxx,lmaxy);
}

void  dbox2d::enclose(const dpos2d& pos1,double tolerance)
{
   if (!m_initialised) {
      m_p1 = pos1;
      m_p2 = m_p1;
      m_initialised = true;
   }
   // recalculate boundaries
   double lminx = min(m_p1.x(),pos1.x()-tolerance);
   double lmaxx = max(m_p2.x(),pos1.x()+tolerance);
   double lminy = min(m_p1.y(),pos1.y()-tolerance);
   double lmaxy = max(m_p2.y(),pos1.y()+tolerance);

   // reset points
   m_p1 = dpos2d(lminx,lminy);
   m_p2 = dpos2d(lmaxx,lmaxy);
}



const dpos2d& dbox2d::p1() const
{
   return m_p1;
}

const dpos2d& dbox2d::p2() const
{
   return m_p2;
}

bool dbox2d::initialised() const
{
   return m_initialised;
}

