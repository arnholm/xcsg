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

#include "polymesh3d.h"

polymesh3d::polymesh3d()
{}

polymesh3d::~polymesh3d()
{}

polymesh3d::polymesh3d(std::shared_ptr<const polymesh2d> pm2d, const carve::math::Matrix& t)
: m_face(pm2d->faces())
, m_contour(pm2d->contours())
{
   m_vert.reserve(pm2d->nvertices());
   for(size_t i=0; i<pm2d->nvertices(); i++) {
      const dpos2d& v = pm2d->vertex(i);
      m_vert.push_back(t * carve::geom::VECTOR(v.x(),v.y(),0.0));
   }
}

// return number of vertices
size_t polymesh3d::nvertices() const
{
   return m_vert.size();
}

// return number of faces
size_t polymesh3d::nfaces() const
{
   return m_face.size();
}

// return number of contours
size_t polymesh3d::ncontours() const
{
   return m_contour.size();
}

const xvertex& polymesh3d::vertex(size_t ivertex) const
{
   return m_vert[ivertex];
}

const polymesh3d::index_vector& polymesh3d::face(size_t iface) const
{
   return m_face[iface];
}

const polymesh3d::index_vector&  polymesh3d::contour(size_t icontour) const
{
   return m_contour[icontour];
}
