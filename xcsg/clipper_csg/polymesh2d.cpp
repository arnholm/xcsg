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

#include "polymesh2d.h"

polymesh2d::polymesh2d()
{}

polymesh2d::~polymesh2d()
{}

// return number of vertices
size_t polymesh2d::nvertices() const
{
   return m_vert.size();
}

// return number of faces
size_t polymesh2d::nfaces() const
{
   return m_face.size();
}

// return number of contours
size_t polymesh2d::ncontours() const
{
   return m_contour.size();
}

void polymesh2d::add_contour(std::shared_ptr<const contour2d> contour)
{
   // get current number of vertices to use as index offset
   size_t icv_offset = m_vert.size();
   size_t ncv        = contour->size();

   // reserve space for more vertices
   m_vert.reserve(icv_offset+ncv);

   // reserve space in contour index vector
   index_vector index_contour;
   index_contour.reserve(ncv);

   // add vertices and contour indices
   for(size_t icv=0; icv<contour->size(); icv++) {
      // get the vertex and add to the mesh
      const dpos2d& vtx = (*contour)[icv];
      m_vert.push_back(vtx);

      // add contour index for this vertex
      index_contour.push_back(icv_offset + icv);
   }

   // add the contour index vector to the vector of contours
   m_contour.push_back(index_contour);
}

void polymesh2d::add_face(const index_vector& face)
{
   m_face.push_back(face);
}

const dpos2d& polymesh2d::vertex(size_t ivertex) const
{
   return m_vert[ivertex];
}

const polymesh2d::index_vector& polymesh2d::face(size_t iface) const
{
   return m_face[iface];
}

const polymesh2d::index_vector&  polymesh2d::contour(size_t icontour) const
{
   return m_contour[icontour];
}

const polymesh2d::face_vector&  polymesh2d::faces() const
{
   return m_face;
}

const polymesh2d::contour_vector  polymesh2d::contours() const
{
   return m_contour;
}
