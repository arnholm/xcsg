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

#ifndef XSHAPE_H
#define XSHAPE_H

#include <carve/mesh.hpp>
#include <memory>
typedef carve::geom3d::Vector xvertex;
class cf_xmlNode;

// abstract base class for 2d and 3d objects

class xshape {
public:
   xshape();
   virtual ~xshape();

   virtual size_t nbool();
};

#endif // XSHAPE_H
