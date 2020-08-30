// BeginLicense:
// Part of: dmesh - Delaunay mesh library
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

#include "dedge.h"
#include "dvertex.h"
#include "dcoedge.h"
#include "dtriangle.h"
#include "dloop.h"
#include "dmesh.h"

dedge::dedge(dmesh* mesh, size_t iv1, size_t iv2)
: dentity(mesh)
, m_iv1((iv1<iv2)? iv1 : iv2)
, m_iv2((iv1<iv2)? iv2 : iv1)
{}

dedge::~dedge()
{}

size_t dedge::key(size_t iv1, size_t iv2)
{
   if(iv1 < iv2) return iv1*1000000 + iv2;
   return  iv2*1000000 + iv1;
}

dvec2d dedge::dir() const
{
   const dmesh* mesh = get_mesh();
   const dpos2d& p1 = mesh->get_vertex(m_iv1)->pos();
   const dpos2d& p2 = mesh->get_vertex(m_iv2)->pos();
   return dvec2d(p1,p2);
}

dline2d dedge::line() const
{
   const dmesh* mesh = get_mesh();
   const dpos2d& p1 = mesh->get_vertex(m_iv1)->pos();
   const dpos2d& p2 = mesh->get_vertex(m_iv2)->pos();
   return std::move(dline2d(p1,p2));
}

std::unordered_set<dtriangle*> dedge::triangles() const
{
   std::unordered_set<dtriangle*> triangles;
   for(auto coedge : m_users) {
      if( dtriangle* tri = dynamic_cast<dtriangle*>(coedge->parent()) ) {
         triangles.insert(tri);
      }
   }

   return std::move(triangles);
}

double dedge::length() const
{
   const dmesh* mesh = get_mesh();
   const dpos2d& p1 = mesh->get_vertex(m_iv1)->pos();
   const dpos2d& p2 = mesh->get_vertex(m_iv2)->pos();
   return p1.dist(p2);
}
