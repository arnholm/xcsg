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

#include "csg_vector.h"
#include "csg_scalar.h"
#include <stdexcept>

csg_vector::csg_vector()
: csg_value(0)
{}

csg_vector::csg_vector(const std::vector<std::string>& vec,size_t line_no)
: csg_value(line_no)
{
   for(size_t i=0; i<vec.size(); i++) {
      m_vector.push_back(std::make_shared<csg_scalar>(vec[i],line_no));
   }
}

csg_vector::csg_vector(const std::vector<std::shared_ptr<csg_value>>& vec,size_t line_no)
: csg_value(line_no)
, m_vector(vec)
{}

csg_vector::~csg_vector()
{}


std::shared_ptr<csg_value> csg_vector::get(size_t i) const
{
   if(i >= m_vector.size()) throw std::runtime_error("csg_vector::get(), index out of bounds");

   return m_vector[i];
}
