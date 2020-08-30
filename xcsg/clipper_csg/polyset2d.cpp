// BeginLicense:
// Part of: xcsg - XML based Constructive Solid Geometry
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

#include "polyset2d.h"

polyset2d::polyset2d()
{
   //ctor
}

polyset2d::~polyset2d()
{
   //dtor
}


dbox2d polyset2d::bounding_box() const
{
   dbox2d box;
   for(auto poly : m_poly) {
      dbox2d pbox = poly->bounding_box();
      box.enclose(pbox.p1());
      box.enclose(pbox.p2());
   }
   return box;
}

double polyset2d::greatest_extent() const
{
   dbox2d box = bounding_box();
   const dpos2d& p1 = box.p1();
   const dpos2d& p2 = box.p2();
   double ext = std::max(p2.x()-p1.x(),p2.y()-p1.y());
   return ext;
}
