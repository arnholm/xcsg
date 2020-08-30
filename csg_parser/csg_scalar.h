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

#ifndef CSG_SCALAR_H
#define CSG_SCALAR_H

#include <string>
#include "csg_value.h"

// a scalar type parameter value in a csg node
class csg_scalar : public csg_value {
public:
   csg_scalar();
   csg_scalar(const std::string& value);
   virtual ~csg_scalar();

   size_t size() const { return 1; }

   std::string to_string() const { return m_value; }
   bool to_bool() const;
   int to_int() const;
   double to_double() const;

private:
   std::string m_value;
};

#endif // CSG_SCALAR_H
