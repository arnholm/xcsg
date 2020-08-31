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

#ifndef CSG_VECTOR_H
#define CSG_VECTOR_H

#include "csg_value.h"
#include <vector>

// a scalar vector parameter value in a csg node
class csg_vector : public csg_value {
public:
   csg_vector();
   csg_vector(const std::vector<std::string>& vec,size_t line_no);
   csg_vector(const std::vector<std::shared_ptr<csg_value>>& vec,size_t line_no);
   virtual ~csg_vector();

   virtual bool is_vector() const { return true; }

   // length of value vector, 1 for scalars
   virtual size_t size() const { return m_vector.size(); }

   // returns reference to value index i for vectors
   virtual std::shared_ptr<csg_value> get(size_t i) const;

private:
   std::vector<std::shared_ptr<csg_value>> m_vector;
};

#endif // CSG_VECTOR_H
