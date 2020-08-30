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

#ifndef XSHAPE2D_H
#define XSHAPE2D_H


#include "xshape.h"
#include <carve/matrix.hpp>
#include "clipper_csg/clipper_profile.h"

// abstract base class for 2d objects

class xshape2d : public xshape {
public:
   xshape2d();
   virtual ~xshape2d();

   void set_transform(const cf_xmlNode& parent);
   const carve::math::Matrix& get_transform() const;

   virtual std::shared_ptr<carve::mesh::MeshSet<3>> create_carve_mesh(const carve::math::Matrix& t = carve::math::Matrix()) const = 0;
   virtual std::shared_ptr<clipper_profile> create_clipper_profile(const carve::math::Matrix& t = carve::math::Matrix()) const = 0;
private:
   carve::math::Matrix m_t;
};

#endif // XSHAPE2D_H
