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

#ifndef DPOS2D_H
#define DPOS2D_H

#include "dvec2d.h"

// a position in 2d space

class dpos2d {
public:
   dpos2d();
   dpos2d(double x,double y);
   dpos2d(const dpos2d& pos);
  ~dpos2d();

   inline double x() const { return m_x; }
   inline double y() const { return m_y; }

   void x( double x );
   void y( double y );

   dpos2d& operator =(const dpos2d& pos);
   dpos2d& operator+=(const dvec2d& vec);
   dpos2d& operator-=(const dvec2d& vec);

   dpos2d& operator+=(const dpos2d& pos);
   dpos2d& operator-=(const dpos2d& pos);

   double dist(const dpos2d& pos) const;
   double dist_squared(const dpos2d& pos) const;

 	friend dpos2d operator+(const dpos2d& pos, const dvec2d& vec);
	friend dpos2d operator+(const dvec2d& vec, const dpos2d& pos);

 	friend dpos2d operator-(const dpos2d& pos, const dvec2d& vec);
	friend dpos2d operator-(const dvec2d& vec, const dpos2d& pos);

   friend dpos2d operator*(const dpos2d& pos, double value);
	friend dpos2d operator*(double value, const dpos2d& pos);

 	friend dvec2d operator-(const dpos2d& pos1, const dpos2d& pos2);

private:
   double m_x;
   double m_y;
};

//Operators
inline dpos2d operator+(const dpos2d& pos, const dvec2d& vec)   { return dpos2d(pos.m_x + vec.x(), pos.m_y + vec.y());   }
inline dpos2d operator+(const dvec2d& vec, const dpos2d& pos)   { return dpos2d(pos.m_x + vec.x(), pos.m_y + vec.y());   }
inline dpos2d operator-(const dpos2d& pos, const dvec2d& vec)   { return dpos2d(pos.m_x - vec.x(), pos.m_y - vec.y());   }
inline dpos2d operator-(const dvec2d& vec, const dpos2d& pos)   { return dpos2d(-pos.m_x + vec.x(), -pos.m_y + vec.y()); }
inline dpos2d operator*(const dpos2d& pos, double value)       { return dpos2d(pos.x()*value,pos.y()*value); }
inline dpos2d operator*(double value, const dpos2d& pos)       { return dpos2d(value*pos.x(),value*pos.y()); }
inline dvec2d operator-(const dpos2d& pos1, const dpos2d& pos2) { return dvec2d(pos2,pos1); }

inline dpos2d operator+(const dpos2d& pos1, const dpos2d& pos2) {return dpos2d(pos1)+=pos2;}

#endif
