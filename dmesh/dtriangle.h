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

#ifndef DTRIANGLE_H
#define DTRIANGLE_H

#include <cstddef>
#include <vector>

#include "dentity.h"
#include "dcircle.h"
#include "dcoedge.h"
class dline2d;
class dedge;

// Topology
// ========
// A dtriangle is a mesh triangle with its 3 sides defined by coedges
// it also contains a circle defined by the 3 triangle vertices.
// Triangles shall be defined with CCW vertex order, giving positive signed area.

class dtriangle : public dentity {
public:
   friend class dmesh;

   // return the signed area of the triangle, where positive area means CCW vertex order
   double signed_area() const;

   // return coedge by index [0..2]
   dcoedge* coedge(size_t i);
   const dcoedge* coedge(size_t i) const;

   // convenient access to the 3 vertices of the triangle
   size_t vertex1() const { return m_coedges[0]->vertex1(); }
   size_t vertex2() const { return m_coedges[1]->vertex1(); }
   size_t vertex3() const { return m_coedges[2]->vertex1(); }

   // return true if 'pos' is inside the triangle circumcircle
   bool in_circumcircle(const dpos2d& pos);

   // return true if 'pos' is inside the triangle (more expensive than in_circumcircle(...))
   bool in_triangle(const dpos2d& pos);

   // return circumcircle
   const dcircle& circle() const { return m_circle; }

   // return the centroid of the triangle
   dpos2d centroid() const;

   // return the aspect ratio shortest/longest side
   double aspect_ratio() const;

   // return the first free coedge edge found, if any. May return NULL
   const dcoedge* free_coedge() const;

   // return the first coedge connected to a reversed loop, if any.  May return NULL
   const dcoedge* reversed_loop() const;

   // return vertex opposite given edge
   size_t oppsite_vertex(const dedge* edge) const;

   // check if line intersects line
   bool intersects(const dline2d& line) const;

protected:
   // declare this triangle a supertriangle. It adds a zero reference to its edges
   void super();

   // return container with all triangle edges
   std::vector<dedge*> get_edges() const;

   // create coedge in this triangle
   dcoedge* create_coedge(size_t iv1, size_t iv2);

   dtriangle(dmesh* mesh, size_t iv1,size_t iv2,size_t iv3);
   virtual ~dtriangle();

   void clear();

private:
   std::vector<dcoedge*> m_coedges;  // the 3 edges of the triangle
   dcircle          m_circle;   // the circumcircle of the triangle
};

#endif // DTRIANGLE_H
