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

#ifndef XTMATRIX_H
#define XTMATRIX_H

#include <carve/matrix.hpp>
class cf_xmlNode;

class xtmatrix {
public:
   xtmatrix();
   xtmatrix(carve::math::Matrix& t);
   xtmatrix(cf_xmlNode& node);
   virtual ~xtmatrix();

   const carve::math::Matrix& t() const;

private:
   carve::math::Matrix m_t;
};

#endif // XTMATRIX_H
