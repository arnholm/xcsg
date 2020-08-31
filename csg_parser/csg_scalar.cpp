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

#include "csg_scalar.h"
#include <stdexcept>
#include <boost/lexical_cast.hpp>

csg_scalar::csg_scalar()
: csg_value(0)
{}

csg_scalar::csg_scalar(const std::string& value,size_t line_no)
: csg_value(line_no)
, m_value(value)
{}

csg_scalar::~csg_scalar()
{}

bool csg_scalar::to_bool() const
{
   return (m_value=="true")? true : false;
}

int csg_scalar::to_int() const
{
   return boost::lexical_cast<int>(m_value);
}

double csg_scalar::to_double() const
{
   return boost::lexical_cast<double>(m_value);
}
