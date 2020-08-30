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

#ifndef XROTATE_EXTRUDE_H
#define XROTATE_EXTRUDE_H

#include "xsolid.h"
#include "xshape2d.h"

class xrotate_extrude : public xsolid {
public:
   xrotate_extrude(double angle);
   xrotate_extrude(const cf_xmlNode& node);
   virtual ~xrotate_extrude();

   virtual size_t nbool();

   std::shared_ptr<carve::mesh::MeshSet<3>> create_carve_mesh(const carve::math::Matrix& t = carve::math::Matrix()) const;
private:
   double  m_angle; // ccw angle around y
   double  m_pitch;
   std::unordered_set<std::shared_ptr<xshape2d>> m_incl;
};

#endif // XROTATE_EXTRUDE_H
