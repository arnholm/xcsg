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
   
#ifndef SWEEP_MESH_H
#define SWEEP_MESH_H


#include <memory>
#include <vector>
#include "xpolyhedron.h"
#include "sweep_path.h"
#include "polymesh3d.h"

class sweep_mesh {
public:
   sweep_mesh(std::shared_ptr<sweep_path> path, bool torus);
   virtual ~sweep_mesh();

   bool sweep();

   std::shared_ptr<xpolyhedron> polyhedron();

private:
   void add_mesh_vertices(std::shared_ptr<const polymesh3d> mesh);
   void add_mesh_faces(size_t v_offset, std::shared_ptr<const polymesh3d> mesh, bool reverse);

   void create_side_faces(size_t v_offset0, // vertex offset to bottom layer vertices
                          size_t v_offset1, // vertex offset to top layer vertices
                          std::shared_ptr<const polymesh3d> mesh, bool reverse);

private:
   std::shared_ptr<sweep_path>  m_path;
   std::shared_ptr<xpolyhedron> m_polyhedron;
   bool                         m_torus;  // true when the final topology shall become a torus
};

#endif // SWEEP_MESH_H
