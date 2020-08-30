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

#ifndef VMAP2D_H
#define VMAP2D_H

#include "dmesh/dpos2d.h"
#include "dmesh/dline2d.h"
#include "contour2d.h"
#include <map>
#include <vector>
// vmap2d implements a closed contour where the vertices have parameters [0,1]
// The lowest  parameter value is always = 0.0
// The highest parameter value is always < 1.0, since the last vertex is the same as the first

class vmap2d {
public:
   typedef std::map<double,dpos2d> Vmap;
   typedef Vmap::iterator iterator;

   typedef std::vector<std::pair<int,int>> EdgeVec; // indices into m_contour
   typedef std::vector<dline2d> Vlines;              // lines from geometric center to vertices

   vmap2d(const contour2d& c);
   virtual ~vmap2d();

   // append vertex lines from this contour to "lines"
   void append_vlines(Vlines& lines) const;

   // imprint all lines into this contour, returns number of contour vertices after imprint
   size_t imprint(Vlines& lines, double epspnt);

   void add_vertices(size_t nv);

   // force the vertex position defined by intersection libe to become first point on contour (or any point being close to it)
   // this might "rotate" the topology of the contained contour
   bool force_first_vertex(const dline2d& vline, double epspnt);

   // return contour
   const contour2d& contour() const;

protected:
   void compute_map();
   void compute_contour();
   void get_edges(EdgeVec& edges) const;

private:
   contour2d m_contour;
   double    m_len;
   Vmap      m_vmap;
};

#endif // VMAP2D_H
