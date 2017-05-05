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
   
#ifndef DBOX2D_H
#define DBOX2D_H

#include "dpos2d.h"

// a dbox2d is a utility for computing a 2d bounding box

class dbox2d {
public:
   dbox2d(bool initialised = false);
   dbox2d(const dpos2d& pos1, const dpos2d& pos2);
   virtual ~dbox2d();

   bool initialised() const;

   dbox2d& operator =(const dbox2d& box);

   void  enclose(const dpos2d& pos1, double tolerance=0.0);

   const dpos2d& p1() const;
   const dpos2d& p2() const;

private:
   void setBox(const dpos2d& pos1, const dpos2d& pos2);

private:
   dpos2d  m_p1;
   dpos2d  m_p2;
   bool   m_initialised;
};

#endif
