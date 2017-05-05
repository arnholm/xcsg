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
   
#include "polygon2d.h"

polygon2d::polygon2d()
{
   //ctor
}

polygon2d::~polygon2d()
{
   //dtor
}

void polygon2d::reserve(size_t ncontour)
{
   m_contours.reserve(ncontour);
}

void polygon2d::push_back(std::shared_ptr<contour2d> contour)
{
   m_contours.push_back(contour);
}

size_t polygon2d::size() const
{
   return m_contours.size();
}

std::shared_ptr<const contour2d> polygon2d::operator[](size_t i) const
{
   return m_contours[i];
}


std::shared_ptr<ClipperLib::Paths> polygon2d::paths() const
{
   std::shared_ptr<ClipperLib::Paths> cpaths(new ClipperLib::Paths());
   cpaths->reserve(m_contours.size());

   for(size_t i=0;i<m_contours.size();i++) {
      cpaths->push_back(m_contours[i]->path());
   }

   return cpaths;
}

bool polygon2d::make_compatible(polygon2d& a, polygon2d& b, double epspnt)
{
   if( a.size() != b.size() ) {
      throw std::logic_error("polygon2d::make_compatible: polygons not compatible' - must have same number of contours");
   }

   size_t nc = a.size();
   for(size_t ic=0;ic<nc; ic++) {
      contour2d::make_compatible(*(a.m_contours[ic]),*(b.m_contours[ic]),epspnt);
   }

   return true;
}
