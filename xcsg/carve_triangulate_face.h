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

#ifndef CARVE_TRIANGULATE_FACE_H
#define CARVE_TRIANGULATE_FACE_H

#include <carve/csg.hpp>
#include <memory>
struct TESStesselator;

class carve_triangulate_face {
public:
   struct spec {
      std::vector<size_t>            vind; // input face vertex indices (into polyhedron vertex vector)
      std::vector<carve::geom2d::P2> vxy;  // projected 2d vertex coordinates
   };

   struct triangles {
      //  a vector of triangulated faces in the form of vertex indices
      std::vector<std::vector<size_t>> tri_faces;
   };

   carve_triangulate_face(std::shared_ptr<spec>  spec);
   virtual ~carve_triangulate_face();

   std::shared_ptr<triangles> compute();

protected:

   // returns a vector of triangulated faces in the form of vertex indices
   std::shared_ptr<triangles> compute2d(const std::vector<size_t>&    vind,
                                        const std::vector<carve::geom2d::P2>& vxy);

protected:
   void create_tess();
   void delete_tess();

private:
   TESStesselator*                   m_tess;
   std::shared_ptr<spec>             m_spec;
};

#endif // CARVE_TRIANGULATE_FACE_H
