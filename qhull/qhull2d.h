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
   
#ifndef QHULL2D_H
#define QHULL2D_H

#include <vector>
#include <map>

// compute convex 2d hull from a set of input coordinates
// the resulting hull contour is guaranteed to be CCW

class qhull2d {
public:
   // local struct for 2d coordinates
   struct xy {
      xy() : x(0), y(0) {}
      xy(double x_, double y_) : x(x_), y(y_) {}
      double x;
      double y;
   };

   typedef std::vector<xy>            vertex_vector;
   typedef vertex_vector::iterator    vertex_iterator;

   qhull2d();
   virtual ~qhull2d();

   // clear all
   void clear();

   // reserve space for number of input vertices
   void reserve(size_t nvert);

   // push an input vertex,
   void push_back(const xy& pnt);

   // compute the convex hull, contour will be properly oriented CCW
   bool compute();

   // contour vertex results below (defined after calling compute)
   size_t vertex_size() const;

   // direct access to vertex by index. Increasing index is CCW order
   const xy& vertex(size_t ivert) { return m_contour[ivert]; }

   // vertex traversal in CCW contour order
   vertex_iterator vertex_begin();
   vertex_iterator vertex_end();

private:
   double signed_area() const;

private:
   std::vector<double>  m_in_vert;   // input vertices as flat vector {x1,y1,z1,x2,y2,z2,....,xn,yn,zn}
   vertex_vector        m_contour;   // hull contour
};

#endif // QHULL3D_H
