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

#include "geodesic_sphere.h"

geodesic_sphere::geodesic_sphere(size_t idepth)
{
   init(idepth);
   m_vert.shrink_to_fit();
   m_faces.shrink_to_fit();
}

geodesic_sphere::~geodesic_sphere()
{
   //dtor
}

size_t  geodesic_sphere::v_size() const
{
   return m_vert.size();
}

const xvertex& geodesic_sphere::v_get(size_t v_ind) const
{
   return m_vert.at(v_ind);
}

size_t geodesic_sphere::f_size() const
{
   return m_faces.size();
}

const xface& geodesic_sphere::f_get(size_t f_ind) const
{
   return m_faces.at(f_ind);
}

void geodesic_sphere::init(size_t idepth)
{
   // just reserve some
   m_vert.reserve(12*3*3);
   m_faces.reserve(20*3*3);

   // start with an Icosahedron
   // The magic numbers X and Z are chosen so that the distance from the origin to any of the vertices of the icosahedron is 1.0.

   const double X = 0.525731112119133606;
   const double Z = 0.850650808352039932;

    // create the 12 iniitial vertices
    m_vert = {
        carve::geom::VECTOR(-  X, 0.0,   Z ),
        carve::geom::VECTOR(   X, 0.0,   Z ),
        carve::geom::VECTOR(  -X, 0.0,  -Z ),
        carve::geom::VECTOR(   X, 0.0,  -Z ),
        carve::geom::VECTOR( 0.0,   Z,   X ),
        carve::geom::VECTOR( 0.0,   Z,  -X ),
        carve::geom::VECTOR( 0.0,  -Z,   X ),
        carve::geom::VECTOR( 0.0,  -Z,  -X ),
        carve::geom::VECTOR(   Z,   X, 0.0 ),
        carve::geom::VECTOR(  -Z,   X, 0.0 ),
        carve::geom::VECTOR(   Z,  -X, 0.0 ),
        carve::geom::VECTOR(  -Z,  -X, 0.0 )
    };

    // the 20 initial icosahedron faces as vertex indices
    // these are just the starting faces, they will not become faces if idepth>0
    size_t tindices[20][3] = {
        { 0,  4,  1}, { 0, 9,  4 }, { 9,  5, 4 }, {  4, 5, 8 }, { 4, 8, 1 },
        { 8, 10,  1}, { 8, 3, 10 }, { 5,  3, 8 }, {  5, 2, 3 }, { 2, 7, 3 },
        { 7, 10,  3}, { 7, 6, 10 }, { 7, 11, 6 }, { 11, 0, 6 }, { 0, 1, 6 },
        { 6,  1, 10}, { 9, 0, 11 }, { 9, 11, 2 }, {  9, 2, 5 }, { 7, 2, 11 }
    };

    // traverse the faces of the icosahedron and perform subdivision
    for(size_t i=0; i<20; i++) {
       subdivide(tindices[i][0], tindices[i][1], tindices[i][2], idepth);
    }
}

size_t geodesic_sphere::sub_vertex(size_t iv1, size_t iv2)
{
   size_t vmin = std::min(iv1,iv2);
   size_t vmax = std::max(iv1,iv2);
   size_t key = vmin*1000000 + vmax;
   auto i = m_vmap.find(key);
   if(i != m_vmap.end()) {
      return i->second;
   }
   size_t iv = m_vert.size();
   m_vert.push_back( (m_vert[iv1] + m_vert[iv2]).normalize() );
   m_vmap.insert(std::make_pair(key,iv));
   return iv;
}


void geodesic_sphere::subdivide(size_t iv1, size_t iv2, size_t iv3, size_t idepth)
{
   if(idepth == 0) {
      // create one new face and stop recursion
      m_faces.push_back(xface(iv3,iv2,iv1));
      return;
   }

   // get 3 new vertex indices
   size_t iv12 = sub_vertex(iv1,iv2);
   size_t iv23 = sub_vertex(iv2,iv3);
   size_t iv31 = sub_vertex(iv3,iv1);

   // continue subdivision
   subdivide( iv1, iv12, iv31, idepth - 1);
   subdivide( iv2, iv23, iv12, idepth - 1);
   subdivide( iv3, iv31, iv23, idepth - 1);
   subdivide(iv12, iv23, iv31, idepth - 1);
}
