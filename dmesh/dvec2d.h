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

#ifndef DVEC2D_H
#define DVEC2D_H

class dpos2d;

// a vector in 2d space

class dvec2d {
public:
   dvec2d();
   dvec2d(double x,double y);
   dvec2d(const dpos2d& start,const dpos2d& end);
   dvec2d(const dvec2d& vec);
  ~dvec2d();

   double x() const;
   double y() const;

   dvec2d& operator =(const dvec2d& vec);
   int    operator <(const dvec2d& vec) const;
   dvec2d  operator -() const;

   dvec2d& normalise();
   dvec2d& scale(double factor);
   dvec2d& add(const dvec2d& vec2);
   dvec2d& subtract(const dvec2d& vec2);
   dvec2d& reverse();

   double dot(const dvec2d& vec2) const;
   double cross(const dvec2d& vec2) const;
   double angle(const dvec2d& vec2) const;
   double length() const;
   double length_squared() const;

   dvec2d  normal(int sign) const;

//Vector arithmetic
   dvec2d& operator+=(const dvec2d& vec);
   dvec2d& operator-=(const dvec2d& vec);

   double operator%=(const dvec2d& vec);    //Scalar product

   dvec2d& operator*=(double factor);       //Scaling
   dvec2d operator+(const dvec2d& vec);
   dvec2d operator-(const dvec2d& vec);

   friend dvec2d operator*(const dvec2d& vec,double factor);
   friend dvec2d operator*(double factor,const dvec2d& vec);
   friend dvec2d normalise(const dvec2d& vec);

private:
   double m_x;
   double m_y;
};

inline dvec2d normalise(const dvec2d& vec)                       { return dvec2d(vec).normalise();                     }
inline dvec2d operator*(const dvec2d& vec, double value)         { return dvec2d(vec.x()*value,vec.y()*value);         }
inline dvec2d operator*(double value, const dvec2d& vec)         { return dvec2d(value*vec.x(),value*vec.y());         }
inline dvec2d operator+(const dvec2d& vec1, const dvec2d& vec2)  { return dvec2d(vec1.x()+vec2.x(),vec1.y()+vec2.y()); }
inline dvec2d operator-(const dvec2d& vec1, const dvec2d& vec2)  { return dvec2d(vec1.x()-vec2.x(),vec1.y()-vec2.y()); }

#endif

