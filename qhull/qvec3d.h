// BeginLicense:
// Part of: xcsg - XML based Constructive Solid Gemoetry
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
   
#ifndef QVEC3D_H
#define QVEC3D_H

class qvec3d {
public:
   qvec3d();
   qvec3d(double x,double y,double z);
   qvec3d(const qvec3d& vec);
  ~qvec3d();

   double x() const;
   double y() const;
   double z() const;

   qvec3d  cross(const qvec3d& vec2) const;
   double dot(const qvec3d& vec2) const;
   qvec3d& normalise();
   double length() const;
private:
   double m_x;
   double m_y;
   double m_z;
};

//
//Inline implementation
//

//Construction

inline qvec3d::qvec3d(double x,double y,double z)
: m_x(x),m_y(y),m_z(z)
{}

inline qvec3d::qvec3d()
: m_x(0.0),m_y(0.0),m_z(0.0)
{}

inline qvec3d::qvec3d(const qvec3d& vec)
: m_x(vec.m_x),m_y(vec.m_y),m_z(vec.m_z)
{}

inline qvec3d::~qvec3d()
{}

//Access

inline double qvec3d::x() const {return m_x;}
inline double qvec3d::y() const {return m_y;}
inline double qvec3d::z() const {return m_z;}

#endif

