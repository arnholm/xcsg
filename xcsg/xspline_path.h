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


#ifndef XSPLINE_PATH_H
#define XSPLINE_PATH_H

#include <memory>
class cf_xmlNode;
#include "csplines/spline_path.h"

class xspline_path {
public:
   xspline_path();
   xspline_path(const cf_xmlNode& node);
   virtual ~xspline_path();

   const std::vector<csplines::cpoint>& cp() const;

protected:

private:
   std::vector<csplines::cpoint> m_cp;  // spline control points
};

#endif // XSPLINE_PATH_H
