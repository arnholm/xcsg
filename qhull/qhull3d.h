// BeginLicense:
// Part of: xcsg - XML based Constructive Solid Gemoetry
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

#ifndef QHULL3D_H
#define QHULL3D_H

#include <vector>
#include <map>

#include "qvec3d.h"

// convert convex 3d hull from a set of input coordinates

class qhull3d {
public:
   typedef std::vector<double>            in_coords;
   typedef in_coords::iterator            in_coords_iterator;

   struct xyz {
      xyz() : x(0), y(0),z(0) {}
      xyz(double x_, double y_, double z_) : x(x_), y(y_),z(z_) {}
      void add(const xyz& p) { x+=p.x; y+=p.y; z+=p.z; }
      void scale(double factor) { x*=factor; y*=factor; z*=factor;}
      double x;
      double y;
      double z;
   };

   typedef std::map<size_t,xyz>           vertex_map;
   typedef vertex_map::iterator           vertex_iterator;

   typedef std::vector<size_t>            face_vertices;
   typedef std::map<size_t,face_vertices> face_map;
   typedef face_map::iterator             face_iterator;

   qhull3d();
   virtual ~qhull3d();

   // clear all
   void clear();

   // reserve space for number of vertices
   void reserve(size_t nvert);

   // push an input coordinate
   void push_back(double x, double y, double z);
   in_coords_iterator in_coords_begin();
   in_coords_iterator in_coords_end();

   // compute the convex hull, faces will be properly oriented
   bool compute();

   // number of vertices (defined after calling compute)
   size_t nvertices() const;

   // number of faces (defined after calling compute)
   size_t nfaces() const;

   // vertex traversal. result vertices are different from input!
   vertex_iterator vertex_begin();
   vertex_iterator vertex_end();

   // face traversal
   face_iterator face_begin();
   face_iterator face_end();

private:
   void check_flip(xyz& cen, xyz& face_cen, face_vertices& vert);

   // compute a face normal using Newell's method
   qvec3d newell_face_normal(face_vertices& vert);

private:
   in_coords  m_in_vert;   // input vertices as flat vector {x1,y1,z1,x2,y2,z2,....,xn,yn,zn}

   vertex_map     m_vert;    // result vertices
   face_map       m_faces;   // face id, face vertex indices
};

#endif // QHULL3D_H
