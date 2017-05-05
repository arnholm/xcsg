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
   
#ifndef POLYMESH2D_H
#define POLYMESH2D_H

#include <vector>
#include <memory>
#include "dmesh/dpos2d.h"
#include "contour2d.h"

// polymesh2d can represent the mesh of multiple complex polygons, possibly with holes.
// Contours must be added successively per polygon, in proper CCW/CW order, ref. polygon2d definitions.
// Each polygon is tesselated and faces added (with adjusted vertex offset) to the mesh.

class polymesh2d {
public:
   friend class dmesh_adapter;

   typedef std::vector<dpos2d>      vertex_vector;
   typedef std::vector<size_t>        index_vector;    // a vector of indices into m_vert
   typedef std::vector<index_vector>  face_vector;     // an index_vector per face
   typedef std::vector<index_vector>  contour_vector;  // an index_vector per contour

   polymesh2d();
   virtual ~polymesh2d();

   // return number of vertices
   size_t nvertices() const;

   // return vertex coordinates for given vertex
   const dpos2d& vertex(size_t ivertex) const;

   // return number of faces
   size_t nfaces() const;

   // return vertex index vector for given face
   const index_vector& face(size_t iface) const;

   // return number of contours
   size_t ncontours() const;

   // return vertex index vector for given contour
   const index_vector& contour(size_t icontour) const;

   const face_vector& faces() const;
   const contour_vector contours() const;

protected:

   // add a new contour to the mesh, this adds vertices also
   void add_contour(std::shared_ptr<const contour2d> contour);

   // add a face to the mesh, the index_vector contains vertex indices for the face
   // The indicies must properly refer to vertexes in this mesh
   void add_face(const index_vector& face);

private:
    vertex_vector   m_vert;    // vertices
    face_vector     m_face;    // faces    , each entry contains N indices into m_vert (not limited to 3)
    contour_vector  m_contour; // contours , each entry contains N indices into m_vert
};

#endif // POLYMESH2D_H
