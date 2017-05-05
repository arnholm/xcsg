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
   
#ifndef GEODESIC_SPHERE_H
#define GEODESIC_SPHERE_H

#include "xshape.h"
#include "xface.h"
#include <vector>
#include <map>

// inspired by http://stackoverflow.com/questions/17705621/algorithm-for-a-geodesic-sphere
// rewritten and extended with topology

// geodesic_sphere generates a unit radius geodesic sphere using depth recursion

class geodesic_sphere {
public:
   geodesic_sphere(size_t idepth);
   virtual ~geodesic_sphere();

   // access vertices
   size_t         v_size() const;
   const xvertex& v_get(size_t v_ind) const;

   // access faces
   size_t         f_size() const;
   const xface&   f_get(size_t f_ind) const;

protected:
   void init(size_t idepth);
   void subdivide(size_t v1, size_t v2, size_t v3, size_t idepth);

   size_t sub_vertex(size_t iv1, size_t iv2);

private:
   std::map<size_t,size_t>  m_vmap;    // <key,vertex_index>
   std::vector<xvertex>     m_vert;    // vertex coordinates
   std::vector<xface>       m_faces;   // vertex indices for faces
};

#endif // GEODESIC_SPHERE_H
