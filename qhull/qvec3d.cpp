// BeginLicense:
// Part of: xcsg - XML based Constructive Solid Gemoetry
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

#include "qvec3d.h"
#include <math.h>

qvec3d qvec3d::cross(const qvec3d& vec2) const
{
   double x = m_y*vec2.m_z - m_z*vec2.m_y;
   double y = m_z*vec2.m_x - m_x*vec2.m_z;
   double z = m_x*vec2.m_y - m_y*vec2.m_x;
   return qvec3d(x,y,z);
}

double qvec3d::dot(const qvec3d& vec2) const
{
   return m_x*vec2.m_x + m_y*vec2.m_y + m_z*vec2.m_z;
}

qvec3d& qvec3d::normalise()
{
   double len = length();
   if(len > 0.0) {
      m_x = m_x/len;
      m_y = m_y/len;
      m_z = m_z/len;
   }
   return *this;
}

double qvec3d::length() const
{
   return sqrt(m_x*m_x + m_y*m_y + m_z*m_z);
}
