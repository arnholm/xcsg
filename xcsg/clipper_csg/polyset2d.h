// BeginLicense:
// Part of: xcsg - XML based Constructive Solid Geometry
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

#ifndef POLYSET2D_H
#define POLYSET2D_H

#include "polygon2d.h"
#include <list>

class polyset2d {
public:
   typedef std::list<std::shared_ptr<polygon2d>>  polygon2d_list;
   typedef polygon2d_list::iterator iterator;

   polyset2d();
   virtual ~polyset2d();

   void push_back(std::shared_ptr<polygon2d> poly) { m_poly.push_back(poly); }

   size_t size() const { return m_poly.size(); }
   iterator begin()    { return m_poly.begin(); }
   iterator end()      { return m_poly.end(); }

private:
   polygon2d_list m_poly;
};

#endif // POLYSET2D_H
