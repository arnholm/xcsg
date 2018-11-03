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

#ifndef CARVE_TRIANGULATE_H
#define CARVE_TRIANGULATE_H

#include <vector>
#include <memory>
#include <carve/csg.hpp>
#include <ostream>

class carve_triangulate {
public:
   typedef std::vector<std::shared_ptr<carve::poly::Polyhedron>> poly_vector;

   carve_triangulate();
   virtual ~carve_triangulate();

   // compute triangulated polyhedron and add to polyset
   size_t compute(std::shared_ptr<carve::poly::Polyhedron> poly, bool improve, bool canonicalize, bool degen_check);

   // compute triangulated polyhedron and add to polyset, alternative implementation
   size_t compute2(std::shared_ptr<carve::poly::Polyhedron> poly);

   // add polyhedron to polyset without triangulation
   void add(std::shared_ptr<carve::poly::Polyhedron> poly);

   std::shared_ptr<poly_vector> carve_polyset() { return m_polyset; }

private:
   std::shared_ptr<poly_vector> m_polyset;
};

#endif // CARVE_TRIANGULATE_H
