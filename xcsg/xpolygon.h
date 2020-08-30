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

#ifndef XPOLYGON_H
#define XPOLYGON_H

#include "xshape2d.h"
#include <vector>

class xpolygon : public xshape2d {
public:
   xpolygon();
   xpolygon(const cf_xmlNode& node);
   virtual ~xpolygon();

   // vertices
   void           v_reserve(size_t nverts);
   size_t         v_add(double x, double y);
   size_t         v_size() const;

   std::shared_ptr<clipper_profile> create_clipper_profile(const carve::math::Matrix& t = carve::math::Matrix()) const;

   std::shared_ptr<carve::mesh::MeshSet<3>> create_carve_mesh(const carve::math::Matrix& t = carve::math::Matrix()) const;

private:
   std::vector<xvertex> m_vert;
};

#endif // XRECTANGLE_H
