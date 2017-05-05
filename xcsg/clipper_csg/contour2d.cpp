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
   
#include "contour2d.h"
#include "vmap2d.h"
#include <utility>
#include <map>
#include <limits>

contour2d::contour2d()
{}

contour2d::contour2d(const ClipperLib::Path& cpath)
{
   m_vert.reserve(cpath.size());
   for(size_t i=0; i<cpath.size(); i++) {
      const ClipperLib::IntPoint& v = cpath[i];
      m_vert.push_back(dpos2d(double(v.X)/TO_CLIPPER,double(v.Y)/TO_CLIPPER));
   }
}

contour2d::~contour2d()
{}

void contour2d::clear()
{
   m_vert.clear();
}

void  contour2d::reserve(size_t nvert)
{
   m_vert.reserve(nvert);
}

size_t contour2d::size() const
{
   return m_vert.size();
}

void contour2d::push_back(const dpos2d& vertex)
{
   m_vert.push_back(vertex);
}

const dpos2d& contour2d::operator[](size_t i) const
{
   return m_vert[i];
}

ClipperLib::Path  contour2d::path() const
{
   ClipperLib::Path cpath;
   cpath.reserve(m_vert.size());
   for(size_t i=0; i<m_vert.size(); i++) {
      const dpos2d& v = m_vert[i];
      cpath.push_back(ClipperLib::IntPoint(ClipperLib::cInt(v.x()*TO_CLIPPER),ClipperLib::cInt(v.y()*TO_CLIPPER)));
   }
   return std::move(cpath);
}

double contour2d::length() const
{
   double len = 0.0;
   dpos2d prev = m_vert[m_vert.size()-1];
   for(size_t i=0; i<m_vert.size(); i++) {
      len += m_vert[i].dist(prev);
      prev =  m_vert[i];
   }
   return len;
}

dpos2d contour2d::geometric_center() const
{
   double xmin = +std::numeric_limits<double>::max();
   double xmax = -std::numeric_limits<double>::max();
   double ymin = +std::numeric_limits<double>::max();
   double ymax = -std::numeric_limits<double>::max();

   size_t nv = m_vert.size();
   for(size_t i=0; i<nv; i++) {
     xmin = std::min(xmin,m_vert[i].x());
     xmax = std::max(xmax,m_vert[i].x());
     ymin = std::min(ymin,m_vert[i].y());
     ymax = std::max(ymax,m_vert[i].y());
   }
   return dpos2d(0.5*(xmin+xmax),0.5*(ymin+ymax));
}

bool contour2d::make_compatible(contour2d& a, contour2d& b, double epspnt)
{
   vmap2d vma(a),vmb(b);

   // compute vertex lines fromm both contours and put them all in the same list
   // this will be the basis for computing compatible vertices
   vmap2d::Vlines vlines;
   vma.append_vlines(vlines);
   vmb.append_vlines(vlines);

   size_t na = vma.imprint(vlines,epspnt);
   size_t nb = vmb.imprint(vlines,epspnt);
   if(na != nb) {
      // the imprint caused slightly different topology
      // increase the one with smallest number by simple interpolation
      if(na < nb)vma.add_vertices(nb-na);
      else       vmb.add_vertices(na-nb);
   }

   // extracthe the first contour
   a = vma.contour();

   // make sure the b contour starts roughly in the same direction as the start of the a contour
   dline2d vline(a.geometric_center(),a[0]);
   vmb.force_first_vertex(vline,epspnt);

   // extract the b contour
   b = vmb.contour();

   return true;
}
