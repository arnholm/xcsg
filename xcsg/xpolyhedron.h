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

#ifndef XPOLYHEDRON_H
#define XPOLYHEDRON_H

#include <vector>
#include "xface.h"
#include "xsolid.h"

class xpolyhedron : public xsolid {
public:
   xpolyhedron();
   xpolyhedron(const cf_xmlNode& node);
   xpolyhedron(const xpolyhedron& other);
   virtual ~xpolyhedron();

   // vertices
   void           v_reserve(size_t nverts);
   size_t         v_add(const xvertex& pos);
   size_t         v_size() const;
   const xvertex& v_get(size_t v_ind) const;

   // faces
   void           f_reserve(size_t nfaces);
   size_t         f_add(const xface& face, bool reverse_face);
   size_t         f_size() const;
   const xface&   f_get(size_t f_ind) const;

   bool check_polyhedron(ostream& out, size_t& num_non_tri);

   // create meshset from this polyhedron
   std::shared_ptr<carve::mesh::MeshSet<3>> create_carve_mesh(const carve::math::Matrix& t = carve::math::Matrix()) const;

   std::shared_ptr<carve::poly::Polyhedron> create_carve_polyhedron();

private:
   std::vector<xvertex> m_vertices;  // vertex coordinates
   std::vector<xface>   m_faces;     // vertex indices for faces
};

#endif // XPOLYHEDRON_H
